
import java.math.BigDecimal;
import java.util.ArrayList;

/**
 * Принимает запросы от клиентов
 */
class Bank {
    private final DBConnector db = new DBConnector();
    private final BankServer server;
    private final Checker checker;

    private final DBConnector.Query selectUserById = db.makePreparedQuery(
            "select id, login, password, name from USERS where id = ?");
    private final DBConnector.Query selectAccountsByUserId = db.makePreparedQuery(
            "select id, name, type, amount from A2U " +
                    "join ACCOUNTS A2 on A2U.AID = A2.ID where A2U.\"uid\" = ?");
    private final DBConnector.Query selectTransactionById = db.makePreparedQuery(
            "select id, dt, from_ac, to_ac, amount, status " +
                    "from TRANSACTIONS where id = ?");

    Bank(BankServer server, Checker checker) {
        this.server = server;
        this.checker = checker;
    }

    /**
     * Открывает сессию для пользователя
     *
     * @param login    логин пользователя
     * @param password пароль пользователя
     * @return токен, который пользователь может использовать в других запросах
     */
    String logIn(String login, String password) {
        return checker.beginSession(login, password);
    }

    /**
     * Завершает сессию
     *
     * @param token токен сессии, которую надо завершить
     */
    void logOut(String token) {
        checker.endSession(token);
    }

    /**
     * Возвращает информацию о пользователе
     *
     * @param token токен сессии пользователя
     * @return информация о пользователе
     */
    User getUser(String token) {
        Integer uid = checker.getUidIfSessionValid(token);
        return uid == null ? null : db.selectUser(selectUserById.setArg1(uid));
    }

    /**
     * Возвращает список всез аккаунтов пользователя
     *
     * @param token токен сессии пользователя
     * @return список с информациях об аккаунтах
     */
    ArrayList<Account> getAccounts(String token) {
        Integer uid = checker.getUidIfSessionValid(token);
        return uid == null ? null : db.selectAccounts(selectAccountsByUserId.setArg1(uid));
    }

    /**
     * Переводит деньги с одного счёта на другой, если у пользователя есть права на доятуп к счёту fromAccount
     * и на этом счёте достаточно средств.
     * Если прав нет, то запоминает запрос и откладывает его выполнение до подтверждения перевода пользователем,
     * у которого есть права доступа к счёту.
     *
     * @param fromAccount откуда перевести
     * @param toAccount   куда перевести
     * @param amount      сколько перевести
     * @param token       токен сессии пользователя
     * @return true если перевод успешно выполнен или отложен до подтверждения
     */
    boolean transferMoney(int fromAccount, int toAccount, BigDecimal amount, String token) {
        Integer uid = checker.getUidIfSessionValid(token);
        if (uid == null)
            return false;
        if (checker.hasAccessToAccount(uid, fromAccount, Checker.Rights.FULL))
            return server.transferMoney(fromAccount, toAccount, amount);
        return server.requestTransferMoney(fromAccount, toAccount, amount);
    }

    /**
     * Возвращиет список всех переводов с аккаунтами пользователя
     *
     * @param token токен сессии пользователя
     * @return список с информацией о переводах
     */
    ArrayList<Transaction> getTransactions(String token) {
        Integer uid = checker.getUidIfSessionValid(token);
        if (uid == null)
            return null;
        ArrayList<Account> accounts = db.selectAccounts(selectAccountsByUserId.setArg1(uid));
        ArrayList<Transaction> all_transactions = new ArrayList<>();
        for (Account account : accounts)
            all_transactions.addAll(server.getAccountTransactions(account.num));
        return all_transactions;
    }

    /**
     * Подтверждает запрос на перевод со счёта пользователя
     *
     * @param token токен сессии пользователя
     * @param tid   номер перевода
     * @return true, если перевод успешно подтверждён
     */
    boolean confirmTransaction(String token, int tid) {
        Integer uid = checker.getUidIfSessionValid(token);
        if (uid == null)
            return false;
        Transaction t = db.selectTransaction(selectTransactionById.setArg1(tid));
        return t != null &&
                checker.hasAccessToAccount(uid, t.fromAccount, Checker.Rights.FULL) &&
                server.confirmTransaction(tid);
    }

    /**
     * Отклоняет запрос на перевод со счёта пользователя
     *
     * @param token токен сессии пользователя
     * @param tid
     * @return true, если перевод успешно отменён
     */
    boolean rejectTransaction(String token, int tid) {
        Integer uid = checker.getUidIfSessionValid(token);
        if (uid == null)
            return false;
        Transaction t = db.selectTransaction(selectTransactionById.setArg1(tid));
        return t != null &&
                checker.hasAccessToAccount(uid, t.fromAccount, Checker.Rights.FULL) &&
                server.rejectTransaction(tid);
    }
}
