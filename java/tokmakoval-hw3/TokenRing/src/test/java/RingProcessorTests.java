import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

import java.io.File;
import java.time.Duration;
import java.util.stream.Stream;

import static org.junit.jupiter.api.Assertions.*;

public class RingProcessorTests {

    private final File log = new File("tmp.log");
    private static final int timeoutMillis = 100;
    private static final float timeoutFactor = 1.5f;


    @ParameterizedTest
    @MethodSource("args")
    void processesAllPackagesInFiniteTime(int nodesAmount, int pkgAmount)
    {
        RingProcessor rp = new RingProcessor(nodesAmount, log);
        assertDoesNotThrow(() -> rp.startProcessing());
        assertDoesNotThrow(() -> rp.sendPackages(pkgAmount));

        float avgPathLen = 0.5f * nodesAmount;
        float expectedProcessingTime = timeoutMillis * avgPathLen * pkgAmount;
        assertTimeout(Duration.ofMillis((long)(timeoutFactor * expectedProcessingTime)), () -> {
            rp.waitForAllDelivered();
        });

        assertDoesNotThrow(() -> rp.endProcessing());

        assertEquals(pkgAmount, rp.getTotalSend());
        assertEquals(pkgAmount, rp.getTotalDelivered());
    }

    private static Stream<Arguments> args() {
        int[] nodes = {2, 3, 5, 10, 100};
        int[] pkgs  = {1, 2, 3, 10, 100, 1000};
        Stream.Builder<Arguments> builder = Stream.builder();
        for (int n: nodes)
            for (int p : pkgs)
                builder.add(Arguments.of(n, p));
        return builder.build();
    }

    @Test
    void doesntAllowInvalidState()
    {
        // INITIALIZED
        RingProcessor rp = new RingProcessor(2, log);
        assertThrows(RuntimeException.class, () -> rp.waitForAllDelivered());
        assertThrows(RuntimeException.class, () -> rp.endProcessing());
        assertDoesNotThrow(() -> rp.sendPackages(1));

        // STARTED
        assertDoesNotThrow(() -> rp.startProcessing());
        assertThrows(RuntimeException.class, () -> rp.startProcessing());
        assertDoesNotThrow(() -> rp.sendPackages(1));

        assertDoesNotThrow(() -> rp.waitForAllDelivered());
        assertThrows(RuntimeException.class, () -> rp.startProcessing());
        assertDoesNotThrow(() -> rp.sendPackages(1));

        // ENDED
        assertDoesNotThrow(() -> rp.endProcessing());
        assertThrows(RuntimeException.class, () -> rp.startProcessing());
        assertThrows(RuntimeException.class, () -> rp.sendPackages(1));
        assertThrows(RuntimeException.class, () -> rp.waitForAllDelivered());
        assertThrows(RuntimeException.class, () -> rp.endProcessing());
    }

    @Test
    void canProcessBatches() throws InterruptedException
    {
        RingProcessor rp = new RingProcessor(2, log);
        assertDoesNotThrow(() -> rp.sendPackages(10));
        assertDoesNotThrow(() -> rp.startProcessing());
        assertDoesNotThrow(() -> rp.sendPackages(10));

        Thread.sleep(100);
        assertDoesNotThrow(() -> rp.sendPackages(10));

        Thread.sleep(100);
        assertDoesNotThrow(() -> rp.waitForAllDelivered());

        assertDoesNotThrow(() -> rp.sendPackages(10));

        assertDoesNotThrow(() -> rp.waitForAllDelivered());
        assertDoesNotThrow(() -> rp.waitForAllDelivered());

        assertDoesNotThrow(() -> rp.endProcessing());

        assertEquals(40, rp.getTotalSend());
        assertEquals(40, rp.getTotalDelivered());
    }

    @Test
    void interruptable()
    {
        RingProcessor rp = new RingProcessor(2, log);
        assertDoesNotThrow(() -> rp.sendPackages(10000));
        assertDoesNotThrow(() -> rp.startProcessing());
        assertDoesNotThrow(() -> rp.endProcessing());
        assertEquals(10000, rp.getTotalSend());
        assertNotEquals(10000, rp.getTotalDelivered());
    }
}
