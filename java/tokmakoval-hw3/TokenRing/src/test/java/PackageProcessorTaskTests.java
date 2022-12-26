import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.RepeatedTest;
import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import static java.time.Duration.ofMillis;
import static org.junit.jupiter.api.Assertions.*;


class PackageProcessorTaskTests {

    private static final int timeoutMillis = 100;

    private DataPackage pkg;
    private Queue<DataPackage> processed;
    private Lock lock;
    private Condition condvar;

    private PackageProcessorTask task;

    @BeforeEach
    void prepare()
    {
        pkg = new DataPackage(42, "");
        processed = new ConcurrentLinkedQueue<>();
        lock = new ReentrantLock();
        condvar = lock.newCondition();

        task = new PackageProcessorTask(pkg, processed, lock, condvar);
    }

    @RepeatedTest(5)
    void processingCompletes() {
        assertTimeout(ofMillis(timeoutMillis), () -> task.run());

        assertTrue(lock.tryLock(), "Task didn't unlock mutex");
        lock.unlock();

        assertFalse(processed.isEmpty(), "Task didn't save processed pkg");
    }


    @RepeatedTest(5)
    void notifies() {
        lock.lock();
        assertTimeout(ofMillis(timeoutMillis), () -> {
            new Thread(task).start();
            while (processed.isEmpty())
                condvar.await();
        });
        lock.unlock();
    }
}
