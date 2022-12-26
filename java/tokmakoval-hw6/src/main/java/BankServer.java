import java.math.BigDecimal;
import java.sql.SQLException;
import java.util.ArrayList;

/**
 * Выполняет переводы между счетами
 */
class BankServer {
    private final DBConnector db = new DBConnector();
    private final DBConnector.Query selectTransactions = db.makePreparedQuery(
            "select TRANSACTIONS.id, dt, from_ac, to_ac, TRANSACTIONS.amount, status " +
                    "from ACCOUNTS join TRANSACTIONS " +
                    "on ACCOUNTS.id = from_ac or ACCOUNTS.id = to_ac where ACCOUNTS.id = ?");

    private final DBConnector.Query transferMoney = db.makeCallable("call transfer_money(?, ?, ?, ?)");
    private final DBConnector.Query confirmTransaction = db.makeCallable("call confirm_or_reject(?, ?)");

    /**
     * Переводит деньги (перевод считается автоматически подтверждённым)
     *
     * @param fromAccount откуда перевеси
     * @param toAccount   куда перевести
     * @param amount      сколько перевести
     * @return true, если перевод успешно выполнен
     */
    boolean transferMoney(int fromAccount, int toAccount, BigDecimal amount) {
        return transfer(fromAccount, toAccount, amount, true);
    }

    /**
     * Делает запись об ожидающем подтверждение переводе
     *
     * @param fromAccount откуда перевеси
     * @param toAccount   куда перевести
     * @param amount      сколько перевести
     * @return true если запись сделана успешно
     */
    boolean requestTransferMoney(int fromAccount, int toAccount, BigDecimal amount) {
        return transfer(fromAccount, toAccount, amount, false);
    }

    /**
     * Подтверждает и выполняет отложенный перевод
     *
     * @param tid номер перевода
     * @return true если перевод выполнен успешно
     */
    boolean confirmTransaction(int tid) {
        return confirm(tid, true);
    }

    /**
     * Отменяет отложенный перевод
     *
     * @param tid номер перевода
     * @return true если отмена произошла успешно
     */
    boolean rejectTransaction(int tid) {
        return confirm(tid, false);
    }

    /**
     * Возвращает список переводов со счёта и на счёт
     *
     * @param aid номер счёта
     * @return список с информацией о переводах
     */
    ArrayList<Transaction> getAccountTransactions(int aid) {
        return db.selectTransactions(selectTransactions.setArg1(aid));
    }

    private boolean transfer(int fromAccount, int toAccount, BigDecimal amount, boolean confirmed) {
        if (amount.compareTo(new BigDecimal(0)) <= 0)
            throw new RuntimeException("amount <= 0");
        int status = confirmed ? Transaction.Status.AUTO_CONFIRMED.ordinal() : Transaction.Status.WAIT_CONFIRMATION.ordinal();
        transferMoney.setArg(1, fromAccount).setArg(2, toAccount).setArg(3, amount).setArg(4, status);
        try {
            transferMoney.st.executeQuery();
        } catch (SQLException e) {
            return false;
        }
        return true;
    }

    private boolean confirm(int tid, boolean confirm) {
        int status = confirm ? Transaction.Status.CONFIRMED.ordinal() : Transaction.Status.REJECTED.ordinal();
        confirmTransaction.setArg(1, tid).setArg(2, status);
        try {
            confirmTransaction.st.executeQuery();
        } catch (SQLException e) {
            return false;
        }
        return true;
    }

}
