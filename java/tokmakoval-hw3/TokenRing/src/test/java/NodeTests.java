import org.junit.jupiter.api.*;

import static org.junit.jupiter.api.Assertions.*;

import java.io.File;
import java.time.Duration;

@TestMethodOrder(MethodOrderer.OrderAnnotation.class)
public class NodeTests {

    private static final int setDataTmeoutMillis = 5;
    private static final int timeoutMillis = 100;

    public static class NextNodeMock extends Node {
        NextNodeMock(RingProcessorMock rp){
            super(0, rp);
        }

        @Override
        public void run() {
            // do nothing
        }

        public int gotPackagesCount(){
            return buffer.size();
        }
    }

    public static class RingProcessorMock extends RingProcessor {
        RingProcessorMock() {
            super(2, new File("tmp.log"));
        }

        public final CoordinatorNode _corNode = new CoordinatorNode(0, this);
        public final NextNodeMock _nextNode = new NextNodeMock(this);

        @Override
        public CoordinatorNode corNode() {
            return _corNode;
        }

        @Override
        public Node nextNode(Node n){
            return _nextNode;
        }
    }

    private static final RingProcessorMock rp = new RingProcessorMock();
    private static final Node node = new Node(42, rp);

    private static final int sendBeforeStart = PackageProcessorManager.maxProcessingPackages * 2;
    private static final int sendAfterStart = PackageProcessorManager.maxProcessingPackages * 4;

    @RepeatedTest(sendBeforeStart)
    @Order(0)
    void acceptsPackagesBeforeStart(RepetitionInfo repetitionInfo)
    {
        assertEquals(42, node.getId());
        int dstIdx = (repetitionInfo.getCurrentRepetition() % 2 == 0) ? 42 : 0;
        assertTimeout(Duration.ofMillis(setDataTmeoutMillis), () -> node.setData( new DataPackage(dstIdx, "")));
    }

    @Test
    @Order(1)
    void starts()
    {
        assertDoesNotThrow(() -> node.start());
    }

    @RepeatedTest(sendAfterStart)
    @Order(2)
    void acceptsPackagesAfterStart(RepetitionInfo repetitionInfo)
    {
        assertTrue(node.isAlive(), "alive");
        assertFalse(node.isInterrupted(), "not interrupted");
        int dstIdx = (repetitionInfo.getCurrentRepetition() % 2 == 0) ? 42 : 0;
        assertTimeout(Duration.ofMillis(setDataTmeoutMillis), () -> node.setData( new DataPackage(dstIdx, "")));
    }

    @Test
    @Order(3)
    void not_a_test_Sleep() throws InterruptedException
    {
        int expectedProcessingTime = timeoutMillis * (sendBeforeStart + sendAfterStart) / PackageProcessorManager.maxProcessingPackages;
        Thread.sleep(expectedProcessingTime);
        assertTrue(node.isAlive(), "alive");
        assertFalse(node.isInterrupted(), "not interrupted");
    }


    @Test
    @Order(4)
    void transfersPackages()
    {
        int mustBeTransferedToNext = (sendBeforeStart + sendAfterStart) / 2;
        assertEquals(mustBeTransferedToNext, rp._nextNode.gotPackagesCount());
    }

    @Test
    @Order(5)
    void deliversPackages()
    {
        int mustBeDeliveredToCor = (sendBeforeStart + sendAfterStart) / 2;
        assertEquals(mustBeDeliveredToCor, rp._corNode.deliveredCount());
    }

    @Test
    @Order(6)
    void stops()
    {
        node.interrupt();

        assertTimeout(Duration.ofMillis(timeoutMillis), () -> node.join());
    }
}
