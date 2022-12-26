import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.RepeatedTest;
import org.junit.jupiter.api.Test;

import java.lang.reflect.Field;
import java.util.Collection;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import static java.time.Duration.ofMillis;
import static org.junit.jupiter.api.Assertions.*;

class PackageProcessorManagerTests {

    private static int id = 0;

    private static final int timeoutMillis = 100;

    private Lock lock;
    private Condition condvar;

    private PackageProcessorManager ppm;

    @BeforeEach
    void prepare() {
        lock = new ReentrantLock();
        condvar = lock.newCondition();

        ppm = new PackageProcessorManager(lock, condvar);
    }

    private DataPackage genPkg() {
        return new DataPackage(++id, "lorem ipsum");
    }

    @RepeatedTest(5)
    void canProcess() {
        for (int i = 0; i < PackageProcessorManager.maxProcessingPackages; ++i)
            assertDoesNotThrow(() -> ppm.process(genPkg()));

        Object[] hack = new Object[1];  // because we cannot assign in lambda
        assertDoesNotThrow(() -> {
            Field field = ppm.getClass().getDeclaredField("processed");
            field.setAccessible(true);
            hack[0] = field.get(ppm);
        });
        Collection processed = (Collection) hack[0];

        assertTimeout(ofMillis(timeoutMillis), () -> {
            lock.lock();
            while (processed.size() != PackageProcessorManager.maxProcessingPackages)
                condvar.await();
            lock.unlock();
        });
    }

    @Test()
    void limitsParallelTasks() {
        assertNull(ppm.getProcessedIfComplete());
        assertTrue(ppm.canProcessNow());
        assertEquals(0, ppm.getProcessingCount());

        for (int i = 0; i < PackageProcessorManager.maxProcessingPackages; ++i) {
            assertDoesNotThrow(() -> ppm.process(genPkg()));
            assertEquals(i + 1, ppm.getProcessingCount());
        }

        assertFalse(ppm.canProcessNow());

        assertThrows(RuntimeException.class, () -> ppm.process(genPkg()));

        assertTimeout(ofMillis(timeoutMillis), () -> {
            lock.lock();
            while (ppm.getProcessedIfComplete() == null)
                condvar.await();
            lock.unlock();
        });

        assertTrue(ppm.canProcessNow());
        assertEquals(PackageProcessorManager.maxProcessingPackages - 1, ppm.getProcessingCount());
        assertDoesNotThrow(() -> ppm.process(genPkg()));

        ppm.stop();
    }


}
