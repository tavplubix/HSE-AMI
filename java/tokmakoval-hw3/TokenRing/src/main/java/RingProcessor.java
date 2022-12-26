import java.io.File;
import java.io.IOException;
import java.util.logging.FileHandler;
import java.util.logging.Logger;
import java.util.Random;


/**
 * В конструкторе кольцо инициализируется, то есть создаются все узлы и данные на узлах.
 * В методе {@link RingProcessor#startProcessing()} запускается работа кольца - данные начинают
 * обрабатываться по часовой стрелке. Также производится логгирование в {@link RingProcessor#logfile}.
 */
public class RingProcessor {
    private static final boolean logToStderr = false;
    private static final int minDataSize = 4;
    private static final int maxDataSize = 16;
    private static final String upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    private static final String lower = upper.toLowerCase();
    private static final String digits = "0123456789";
    private static final String alphanum = upper + lower + digits;

    private enum State {
        INITIALIZED,
        STARTED,
        ENDED
    }

    private State state;

    private final int corId;

    private Node[] nodes;

    private int totalSend = 0;

    private final FileHandler logfile;
    private final Logger logger;
    private final Random random;

    /**
     * Создаёт TokenRing
     *
     * @param nodesAmount количество нод (> 1)
     * @param logs        файл для записи логов
     */
    RingProcessor(int nodesAmount, File logs) {
        assert 1 < nodesAmount;

        logger = Logger.getLogger("ringLogger");

        try {
            logfile = new FileHandler(logs.getPath());
        } catch (IOException e) {
            throw new RuntimeException("Cannot open file to wrile log");
        }
        logger.addHandler(logfile);
        logger.setUseParentHandlers(logToStderr);

        random = new Random();
        corId = random.nextInt(nodesAmount);

        nodes = new Node[nodesAmount];
        for (int i = 0; i < nodes.length; ++i) {
            if (i == corId) {
                nodes[i] = new CoordinatorNode(i, this);
                logger.info(String.format("Node %d is coordinator", i));
            } else {
                nodes[i] = new Node(i, this);
            }
        }

        state = State.INITIALIZED;
    }

    /**
     * Возвращает ссылку на координатора
     */
    public CoordinatorNode corNode() {
        return (CoordinatorNode) nodes[corId];
    }

    private DataPackage generatePackage() {
        int destNodeId = random.nextInt(nodes.length);
        int size = minDataSize + random.nextInt(maxDataSize - minDataSize);
        StringBuilder buf = new StringBuilder(size);
        for (int i = 0; i < size; ++i)
            buf.append(alphanum.charAt(random.nextInt(alphanum.length())));
        return new DataPackage(destNodeId, buf.toString());
    }

    public Logger getLogger() {
        return logger;
    }

    /**
     * Запускает рабочие циклы нод, начинается обработка пакетов
     */
    public void startProcessing() {
        if (state != State.INITIALIZED)
            throw new RuntimeException("Cannot start token ring");

        logger.info("Starting nodes...");

        for (Node node : nodes)
            node.start();
        state = State.STARTED;

        logger.info("Nodes started");
    }

    /**
     * Генерирует случайные пакеты и отправляет их случайным нодам
     *
     * @param dataAmount количество пакетов для отправки
     */
    public void sendPackages(int dataAmount) {
        assert 0 <= dataAmount;
        if (state == State.ENDED)
            throw new RuntimeException("Token ring is not started");

        logger.info(String.format("Sending %d new packages to nodes", dataAmount));

        for (int j = 0; j < dataAmount; ++j) {
            int nodeId = random.nextInt(nodes.length);
            logger.info(String.format("Sending new package to node %d", nodeId));
            nodes[nodeId].setData(generatePackage());
        }

        totalSend += dataAmount;

        logger.info("New packages sent to nodes");
    }

    public Node nextNode(Node node) {
        int nextId = (int) node.getId() + 1;
        if (nextId == nodes.length)
            nextId = 0;
        return nodes[nextId];
    }

    /**
     * Ожидает, когда все отправленные пакеты придут на координатор
     */
    public void waitForAllDelivered() {
        if (state != State.STARTED)
            throw new RuntimeException("Token ring is not started");

        logger.info("Waiting for all packages delivered...");

        corNode().corLock.lock();
        try {
            while (corNode().deliveredCount() != totalSend)
                corNode().corCondition.await();
        } catch (InterruptedException e) {
            logger.severe("CRITICAL ERROR: RingProcessor thread unexpectedly interrupted: exiting");
            logfile.close();
            System.exit(1);
        } finally {
            corNode().corLock.unlock();
        }

        logger.info("All packages delivered");
    }

    /**
     * Останавливает рабочие циклы нод, выводит статистику в лог
     */
    public void endProcessing() {
        if (state != State.STARTED)
            throw new RuntimeException("Token ring is not started");

        logger.info("Interrupting nodes...");
        for (Node node : nodes)
            node.interrupt();
        try {
            for (Node node : nodes)
                node.join();
        } catch (InterruptedException e) {
            logger.severe("CRITICAL ERROR: RingProcessor thread unexpectedly interrupted: exiting");
            logfile.close();
            System.exit(1);
        }

        state = State.ENDED;

        logger.info("All nodes interrupted");
        logger.info(String.format("Processed %d of %d packages", corNode().deliveredCount(), totalSend));
        logger.info(String.format("Average time of package processing: %f ns", corNode().averageDeliveryTime()));
        logfile.close();
    }

    public int getTotalSend() {
        return totalSend;
    }

    public int getTotalDelivered() {
        return corNode().deliveredCount();
    }
}
