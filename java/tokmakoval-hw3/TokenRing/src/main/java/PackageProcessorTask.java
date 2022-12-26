import java.util.Queue;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;

/**
 * Задача обработки пакета
 */
public class PackageProcessorTask implements Runnable {
    private static final int processingTimeMillis = 42;

    private final DataPackage pkg;
    private final Queue<DataPackage> processed;
    private final Lock lock;
    private final Condition condvar;

    /**
     * Конструктор
     *
     * @param pkg       пакет, который нужно обработать
     * @param processed очередь обработанных пакетов, куда будет помещёт pkg после завершения обработки
     * @param lock      для кондвара
     * @param condvar   кондвар для уведомления других потоков о завершении обработки
     */
    PackageProcessorTask(DataPackage pkg, Queue<DataPackage> processed, Lock lock, Condition condvar) {
        this.pkg = pkg;
        this.processed = processed;
        this.lock = lock;
        this.condvar = condvar;
    }

    protected void processDataPackage() {
        try {
            // Здесь могла быть логика обработки пакета
            Thread.sleep(processingTimeMillis);
        } catch (InterruptedException e) {
            // А здесь могла быть ваша реклама
        }
    }

    @Override
    public final void run() {
        processDataPackage();
        lock.lock();
        processed.add(pkg);
        condvar.signalAll();
        lock.unlock();
    }


}
