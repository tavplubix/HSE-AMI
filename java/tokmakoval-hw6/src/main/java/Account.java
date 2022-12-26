import java.math.BigDecimal;

/**
 * Хранит данные о счёте
 */
public class Account {
    public enum Type {
        CHECKING, SAVING
    }

    Account(int num, String name, int type, BigDecimal amount) {
        this.num = num;
        this.name = name;
        this.type = Type.values()[type];
        this.amount = amount;
    }

    int num;
    String name;
    Type type;
    BigDecimal amount;
}
