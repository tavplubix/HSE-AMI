import org.junit.jupiter.api.MethodOrderer;
import org.junit.jupiter.api.Order;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.TestMethodOrder;

import java.time.Duration;

import static org.junit.jupiter.api.Assertions.*;

@TestMethodOrder(MethodOrderer.OrderAnnotation.class)
public class CoordinatorNodeTests {

    private static final NodeTests.RingProcessorMock rp = new NodeTests.RingProcessorMock();
    private static final CoordinatorNode corNode = new CoordinatorNode(0, rp);

    private static long timeSum = 0;

    @Test
    @Order(0)
    void acceptsDelivery()
    {
        DataPackage pkg = new DataPackage(0, "");
        assertTimeout(Duration.ofMillis(5), () -> corNode.deliverPackage(pkg));
        assertDoesNotThrow(() -> { timeSum += pkg.deliveryTime(); });
        assertEquals(1, corNode.deliveredCount());
    }

    @Test
    @Order(1)
    void notifiesOnDelivery() throws InterruptedException
    {
        DataPackage pkg = new DataPackage(0, "");
        Thread deliveryThread = new Thread(() -> {
            corNode.deliverPackage(pkg);
        });

        assertEquals(1, corNode.deliveredCount());

        assertTimeout(Duration.ofMillis(10), () -> {
            corNode.corLock.lock();
            deliveryThread.start();
            while (corNode.deliveredCount() == 1)
                corNode.corCondition.await();
            corNode.corLock.unlock();
        });

        assertDoesNotThrow(() -> { timeSum += pkg.deliveryTime(); });
        assertEquals(2, corNode.deliveredCount());
    }

    @Test
    @Order(2)
    void calculatesAvgTime()
    {
        assertEquals((float)timeSum / 2, corNode.averageDeliveryTime());
    }

}
