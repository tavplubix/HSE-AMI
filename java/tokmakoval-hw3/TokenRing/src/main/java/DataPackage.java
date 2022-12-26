
public class DataPackage {
    private final int destinationNode;

    private final String data;

    private final long startTime;

    private Long endTime = null;

    DataPackage(int destinationNode, String data) {
        this.destinationNode = destinationNode;

        this.data = data;

        // Фиксируется время, когда создаётся пакет данных. Необходимо для
        // вычисления времени доставки до узла назначения.
        startTime = System.nanoTime();
    }


    public int getDestinationNode() {
        return destinationNode;
    }

    public String getData() {
        return data;
    }

    public void markDelivered() {
        if (endTime != null)
            throw new RuntimeException("Package already delivered");
        endTime = System.nanoTime();
    }

    public long deliveryTime() {
        if (endTime == null)
            throw new RuntimeException("Package not delivered yet");
        return endTime - startTime;
    }
}
