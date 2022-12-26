import java.util.Stack;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.ReentrantLock;

public class Node extends Thread {
    private final int nodeId;

    /**
     * Если верить документации, то Stack и так thread-safe, BufferStack не нужен
     */
    protected final Stack<DataPackage> buffer = new Stack<>();

    private final RingProcessor owner;

    /**
     * Лок для кондвара
     */
    private final ReentrantLock lock = new ReentrantLock();


    /**
     * Используется в {@link PackageProcessorTask#run()} и {@link Node#setData(DataPackage)} для уведомления ноды
     * о завершении обработки пакета или появлении нового пакета в буффере
     */
    private final Condition condition = lock.newCondition();

    private final PackageProcessorManager processor = new PackageProcessorManager(lock, condition);


    Node(int nodeId, RingProcessor owner) {
        this.nodeId = nodeId;
        this.owner = owner;
    }

    public long getId() {
        return nodeId;
    }

    /**
     * Отправляет пакет этой ноде. Можно вызывать из любого потока.
     */
    public void setData(DataPackage dataPackage) {
        lock.lock();
        buffer.push(dataPackage);
        condition.signalAll();
        lock.unlock();
    }

    private void onPackageProcessed(DataPackage processed) {
        owner.getLogger().info(String.format("Node %d processed package %s", nodeId, processed.getData()));
        if (processed.getDestinationNode() == nodeId) {
            owner.getLogger().info(String.format("Node %d sends package %s to coordinator", nodeId, processed.getData()));
            owner.corNode().deliverPackage(processed);
        } else {
            owner.getLogger().info(String.format("Node %d sends package %s to the next node", nodeId, processed.getData()));
            owner.nextNode(this).setData(processed);
        }
    }

    /**
     * Проверяет, можно ли обработать следующий пакет, т.е. в пулле есть свободные обработчики, а в буфере новые пакеты
     */
    private boolean canProcessNextPackage() {
        return processor.canProcessNow() && !buffer.empty();
    }

    /**
     * Цикл работы узла. Ждёт новых пакетов в буфере, запускает обработку в пулле потоков,
     * отправляет обработанные паеты следующей ноде или координатору
     */
    @Override
    public void run() {
        while (true) {
            lock.lock();

            DataPackage processed = processor.getProcessedIfComplete();

            // Проверяем, есть ли пакеты, которые обработались и ждут отправки или которые можно обработать
            while (processed == null && !canProcessNextPackage()) {
                try {
                    // Ждём перемен
                    condition.await();

                    processed = processor.getProcessedIfComplete();
                } catch (InterruptedException e) {
                    // Нас прервали. Это должно происходить, когда все пакеты доставлены координатору и новых не ожидается, выходим.
                    lock.unlock();
                    owner.getLogger().info(String.format("Node %d about to be stopped. Packages still in buffer: %d. Processing packages: %d",
                            nodeId, buffer.size(), processor.getProcessingCount()));
                    processor.stop();
                    return;
                }
            }

            lock.unlock();

            if (processed != null) {
                onPackageProcessed(processed);
            }

            if (canProcessNextPackage()) {
                DataPackage waitingForProcessing = buffer.pop();
                owner.getLogger().info(String.format("Node %d starts processing package %s", nodeId, waitingForProcessing.getData()));
                processor.process(waitingForProcessing);
            }

        }

    }
}

