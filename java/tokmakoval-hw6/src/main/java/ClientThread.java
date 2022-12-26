import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.Random;

/**
 * Выполняет случайные дейсствия пользователя в отдельном потоке
 */
public class ClientThread extends Thread {
    static final int MAX_ID = 20;
    static final ArrayList<Integer> ACCOUNT_IDS = new ArrayList<>();

    static {
        for (int i = 1; i < MAX_ID; ++i) {
            ACCOUNT_IDS.add(i);
            ACCOUNT_IDS.add(i + 1000);
        }
    }

    static final int AMOUNT = 1000;
    static final int TRANSFERS = 1000;
    static final int RATE = 10;

    private BankServer server = new BankServer();
    private Checker checker = new Checker();
    private Bank bank = new Bank(server, checker);
    private BankClient client = new BankClient(bank);
    private Random rand = new Random();
    private int id;

    ClientThread(int id_) {
        id = id_ % MAX_ID + 1;
    }

    @Override
    public void run() {
        client.logIn("user" + Integer.toString(id), "qwerty" + Integer.toString(id));

        int count = rand.nextInt(TRANSFERS);
        for (int i = 0; i < count; ++i) {
            int from = ACCOUNT_IDS.get(rand.nextInt(ACCOUNT_IDS.size()));
            int to = ACCOUNT_IDS.get(rand.nextInt(ACCOUNT_IDS.size()));
            BigDecimal amount = new BigDecimal(rand.nextDouble() * AMOUNT);

            client.transferMoney(from, to, amount);

            if (rand.nextInt(RATE) == 0)
                client.confirmOrRejectTransactions();
        }

        client.logOut();
    }

}
