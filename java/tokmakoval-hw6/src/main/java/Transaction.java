import java.math.BigDecimal;
import java.util.Date;

/**
 * Хранит данные о транзакции
 */
class Transaction {
    enum Status {
        AUTO_CONFIRMED, WAIT_CONFIRMATION, CONFIRMED, REJECTED
    }

    Transaction(int id, Date dt, int fromAccount, int toAccount, BigDecimal amount, int status) {
        this.id = id;
        this.dt = dt;
        this.fromAccount = fromAccount;
        this.toAccount = toAccount;
        this.amount = amount;
        this.status = Status.values()[status];
    }

    int id;
    Date dt;
    int fromAccount;
    int toAccount;
    BigDecimal amount;
    Status status;
}
