
import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.Random;

/**
 * Отправляет запросы в банк
 */
class BankClient {

    BankClient(Bank bank) {
        this.bank = bank;
    }

    /**
     * Открывает сессию работы с банком для пользователя
     *
     * @param login    логин пользователя
     * @param password пароль
     */
    void logIn(String login, String password) {
        if (token != null)
            bank.logOut(token);
        token = bank.logIn(login, password);
        updateUserInfo();
    }

    /**
     * Закрывает сессию работы с банком
     */
    void logOut() {
        bank.logOut(token);
    }

    /**
     * Получает из банка актуальную информацию о пользователе, его счетах и транзакциях
     */
    void updateUserInfo() {
        user = bank.getUser(token);
        accounts = bank.getAccounts(token);
        transactions = bank.getTransactions(token);
    }

    /**
     * Отправляет запрос на переод денег
     *
     * @param fromAccount откуда перевети (если нет прав - запрос ставится в очередь до подтверждения)
     * @param toAccount   куда перевести
     * @param amount      сколько перевести
     * @return true, если перевод выполнен или поставлен в очередь, иначе false
     */
    boolean transferMoney(int fromAccount, int toAccount, BigDecimal amount) {
        return bank.transferMoney(fromAccount, toAccount, amount, token);
    }

    protected boolean acceptTransferMoney(int fromAccount, int toAccount, BigDecimal amount) {
        Random rand = new Random();
        return rand.nextInt() % 2 == 0;
    }

    /**
     * Ищет в транзакции со счетами пользователя, ожидающие подтверждения.
     * Принимает или отклоняет запросы на переводы от других пользователей.
     */
    void confirmOrRejectTransactions() {
        for (Transaction t : transactions) {

            boolean from_my_account = accounts.stream().anyMatch(a -> a.num == t.fromAccount);
            if (t.status == Transaction.Status.WAIT_CONFIRMATION && from_my_account) {
                if (acceptTransferMoney(t.fromAccount, t.toAccount, t.amount))
                    bank.confirmTransaction(token, t.id);
                else
                    bank.rejectTransaction(token, t.id);
            }
        }
    }

    ArrayList<Account> accounts() {
        return accounts;
    }


    private final Bank bank;
    private User user;
    private ArrayList<Account> accounts;
    private ArrayList<Transaction> transactions;
    private String token;
}
