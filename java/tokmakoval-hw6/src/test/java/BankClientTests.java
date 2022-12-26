import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;

import java.math.BigDecimal;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.time.Duration;

import static org.junit.jupiter.api.Assertions.*;

public class BankClientTests {

    static final Bank bank = new Bank(new BankServer(), new Checker());

    class ClientAlwaysConfirm extends BankClient {
        ClientAlwaysConfirm(Bank bank) {
            super(bank);
        }

        @Override
        protected boolean acceptTransferMoney(int fromAccount, int toAccount, BigDecimal amount) {
            return true;
        }
    }

    @BeforeAll
    static void initDb()
    {
        Main.initDb();
    }

    @Test
    void testClientCanTransfer()
    {
        BankClient client = new BankClient(bank);
        client.logIn("user1", "qwerty1");

        assertTrue(client.transferMoney(1, 1001, new BigDecimal(100)));
        client.updateUserInfo();
        assertEquals(new BigDecimal(900), client.accounts().get(0).amount);
        assertEquals(new BigDecimal(100), client.accounts().get(1).amount);

        assertTrue(client.transferMoney(1, 2, new BigDecimal(100)));
        client.updateUserInfo();
        assertEquals(new BigDecimal(800), client.accounts().get(0).amount);
        assertEquals(new BigDecimal(100), client.accounts().get(1).amount);

        assertTrue(client.transferMoney(2, 1, new BigDecimal(100)));
        client.updateUserInfo();
        assertEquals(new BigDecimal(800), client.accounts().get(0).amount);
        assertEquals(new BigDecimal(100), client.accounts().get(1).amount);

        {
            BankClient client2 = new ClientAlwaysConfirm(bank);
            client2.logIn("user2", "qwerty2");
            assertEquals(new BigDecimal(1100), client2.accounts().get(0).amount);
            assertEquals(new BigDecimal(0), client2.accounts().get(1).amount);

            client2.confirmOrRejectTransactions();
            client2.updateUserInfo();
            assertEquals(new BigDecimal(1000), client2.accounts().get(0).amount);
            assertEquals(new BigDecimal(0), client2.accounts().get(1).amount);
        }

        client.updateUserInfo();
        assertEquals(new BigDecimal(900), client.accounts().get(0).amount);
        assertEquals(new BigDecimal(100), client.accounts().get(1).amount);
    }

}
