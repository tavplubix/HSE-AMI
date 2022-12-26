import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;

/**
 * Управляет обработкой пакетов в пулле отдельных потоков.
 * Ограничивает количество одновременно обрабатываемых пакетов до maxProcessingPackages.
 * Для каждого объекта методы должны вызываться только из одного потока
 */
public class PackageProcessorManager {
    public static final int maxProcessingPackages = 3;

    private final ExecutorService executor;
    private final ConcurrentLinkedQueue<DataPackage> processed;
    private final Lock lock;
    private final Condition condvar;

    private int processingCount = 0;

    PackageProcessorManager(Lock lock, Condition condvar) {
        processed = new ConcurrentLinkedQueue<>();
        executor = Executors.newFixedThreadPool(maxProcessingPackages);
        this.lock = lock;
        this.condvar = condvar;
    }

    public void process(DataPackage pkg) {
        if (!canProcessNow())
            throw new RuntimeException("Cannot process package: all threads are busy");
        executor.execute(new PackageProcessorTask(pkg, processed, lock, condvar));

        ++processingCount;
    }

    public int getProcessingCount() {
        return processingCount;
    }

    public boolean canProcessNow() {
        return processingCount < maxProcessingPackages;
    }

    public DataPackage getProcessedIfComplete() {
        DataPackage pkg = processed.poll();
        if (pkg != null)
            --processingCount;
        return pkg;
    }

    public void stop() {
        executor.shutdownNow();
    }

}
