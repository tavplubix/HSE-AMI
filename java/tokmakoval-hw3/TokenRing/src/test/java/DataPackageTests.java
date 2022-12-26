import org.junit.jupiter.api.RepeatedTest;
import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;


class DataPackageTests {

    private final String data = "lorem ipsum";
    private final int node = 42;

    @Test
    void packageLifecycle() {
        DataPackage pkg = new DataPackage(node, data);

        assertEquals(node, pkg.getDestinationNode());
        assertEquals(data, pkg.getData());

        assertThrows(RuntimeException.class, () -> pkg.deliveryTime());

        pkg.markDelivered();

        assertDoesNotThrow(() -> pkg.deliveryTime());
        assertThrows(RuntimeException.class, () -> pkg.markDelivered());

        assertEquals(node, pkg.getDestinationNode());
        assertEquals(data, pkg.getData());
    }

    @RepeatedTest(5)
    void deliveryTime() {
        long beforeConstructed = System.nanoTime();
        DataPackage pkg = new DataPackage(node, data);
        long afterConstructed = System.nanoTime();

        Thread.yield();

        long beforeDelivered = System.nanoTime();
        pkg.markDelivered();
        long afterDelivered = System.nanoTime();

        long time = pkg.deliveryTime();

        Thread.yield();

        assertEquals(time, pkg.deliveryTime());

        long lowerBound = beforeDelivered - afterConstructed;
        long upperBound = afterDelivered - beforeConstructed;

        assertTrue(lowerBound <= time);
        assertTrue(time <= upperBound);

    }
}
