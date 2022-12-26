import java.io.File;


public class Main {

    public static void main(String[] args) {
        RingProcessor processor = new RingProcessor(20, new File("logPath"));

        processor.startProcessing();
        processor.sendPackages(100);
        processor.waitForAllDelivered();
        processor.sendPackages(100);
        processor.sendPackages(100);
        processor.waitForAllDelivered();
        processor.endProcessing();
    }
}
