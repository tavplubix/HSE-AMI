import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.ReentrantLock;

/**
 * Нода-координатор. Получает пакеты с конечных нод,
 * предоставляет кондвар для ожидания получения пакетов с конечных нод другими потоками
 */
public class CoordinatorNode extends Node {

    private List<DataPackage> allData;

    public final ReentrantLock corLock = new ReentrantLock();
    /**
     * Позволяет пробуждать другие потоки, когда на координатор пришёл пакет.
     * Используется в {@link RingProcessor#waitForAllDelivered()}
     */
    public final Condition corCondition = corLock.newCondition();

    CoordinatorNode(int nodeId, RingProcessor owner) {
        super(nodeId, owner);
        allData = new ArrayList<>();
    }

    /**
     * Получает пакет с конечной ноды. Можно вызывать из любого потока
     */
    public void deliverPackage(DataPackage pkg) {
        pkg.markDelivered();
        corLock.lock();
        allData.add(pkg);
        corCondition.signalAll();
        corLock.unlock();
    }

    public int deliveredCount() {
        return allData.size();
    }

    public float averageDeliveryTime() {
        float sum = 0;
        for (DataPackage p : allData)
            sum += p.deliveryTime();
        return sum / allData.size();
    }
}
