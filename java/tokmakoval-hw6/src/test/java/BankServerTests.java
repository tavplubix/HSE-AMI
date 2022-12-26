import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import java.math.BigDecimal;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.time.Duration;
import java.util.ArrayList;

import static org.junit.jupiter.api.Assertions.*;


public class BankServerTests
{
    @BeforeEach
    void initDb() {
        Main.initDb();
    }

    @Test
    void testBadAmount() {
        BankServer server = new BankServer();
        assertThrows(RuntimeException.class, () -> server.transferMoney(1, 2, new BigDecimal(-1)));
        assertThrows(RuntimeException.class, () -> server.requestTransferMoney(1, 2, new BigDecimal(-1)));
        assertFalse(server.transferMoney(1, 2, new BigDecimal(10000)));
    }

    @Test
    void testTransfer() {
        BankServer server = new BankServer();
        assertThrows(RuntimeException.class, () -> server.transferMoney(1, 2, new BigDecimal(-1)));
        assertFalse(server.transferMoney(1, 2, new BigDecimal(10000)));
        assertTrue(server.transferMoney(1, 2, new BigDecimal(100)));

        ArrayList<Transaction> ts = server.getAccountTransactions(1);
        assertEquals(1, ts.size());

        Transaction t1 = ts.get(0);
        assertEquals(1, t1.fromAccount);
        assertEquals(2, t1.toAccount);
        assertEquals(new BigDecimal(100), t1.amount);
        assertEquals(Transaction.Status.AUTO_CONFIRMED, t1.status);

        ts = server.getAccountTransactions(2);
        assertEquals(1, ts.size());

        Transaction t2 = ts.get(0);
        assertEquals(1, t1.fromAccount);
        assertEquals(2, t1.toAccount);
        assertEquals(new BigDecimal(100), t1.amount);
        assertEquals(Transaction.Status.AUTO_CONFIRMED, t1.status);

        assertFalse(server.confirmTransaction(t1.id));
        assertFalse(server.rejectTransaction(t1.id));

        DBConnector db = new DBConnector();
        Account a1 = db.selectAccount(db.makePreparedQuery("select * from ACCOUNTS where id = 1"));
        Account a2 = db.selectAccount(db.makePreparedQuery("select * from ACCOUNTS where id = 2"));
        assertEquals(new BigDecimal(900), a1.amount);
        assertEquals(new BigDecimal(1100), a2.amount);
    }


    @Test
    void testTransferConfirmed() {
        BankServer server = new BankServer();
        assertTrue(server.requestTransferMoney(1, 2, new BigDecimal(100)));

        ArrayList<Transaction> ts = server.getAccountTransactions(1);
        assertEquals(1, ts.size());

        Transaction t1 = ts.get(0);
        assertEquals(Transaction.Status.WAIT_CONFIRMATION, t1.status);

        DBConnector db = new DBConnector();
        Account a1 = db.selectAccount(db.makePreparedQuery("select * from ACCOUNTS where id = 1"));
        Account a2 = db.selectAccount(db.makePreparedQuery("select * from ACCOUNTS where id = 2"));
        assertEquals(new BigDecimal(1000), a1.amount);
        assertEquals(new BigDecimal(1000), a2.amount);

        assertTrue(server.confirmTransaction(t1.id));
        assertFalse(server.confirmTransaction(t1.id));
        assertFalse(server.rejectTransaction(t1.id));

        ts = server.getAccountTransactions(1);
        assertEquals(1, ts.size());

        t1 = ts.get(0);
        assertEquals(Transaction.Status.CONFIRMED, t1.status);


        a1 = db.selectAccount(db.makePreparedQuery("select * from ACCOUNTS where id = 1"));
        a2 = db.selectAccount(db.makePreparedQuery("select * from ACCOUNTS where id = 2"));
        assertEquals(new BigDecimal(900), a1.amount);
        assertEquals(new BigDecimal(1100), a2.amount);
    }

    @Test
    void testTransferConfirmedNotEnoughMoney() {
        BankServer server = new BankServer();
        assertTrue(server.requestTransferMoney(1, 2, new BigDecimal(10000)));

        ArrayList<Transaction> ts = server.getAccountTransactions(1);
        assertEquals(1, ts.size());

        Transaction t1 = ts.get(0);
        assertEquals(Transaction.Status.WAIT_CONFIRMATION, t1.status);

        DBConnector db = new DBConnector();
        Account a1 = db.selectAccount(db.makePreparedQuery("select * from ACCOUNTS where id = 1"));
        Account a2 = db.selectAccount(db.makePreparedQuery("select * from ACCOUNTS where id = 2"));
        assertEquals(new BigDecimal(1000), a1.amount);
        assertEquals(new BigDecimal(1000), a2.amount);

        assertFalse(server.confirmTransaction(t1.id));

        ts = server.getAccountTransactions(1);
        assertEquals(1, ts.size());

        t1 = ts.get(0);
        assertEquals(Transaction.Status.WAIT_CONFIRMATION, t1.status);


        a1 = db.selectAccount(db.makePreparedQuery("select * from ACCOUNTS where id = 1"));
        a2 = db.selectAccount(db.makePreparedQuery("select * from ACCOUNTS where id = 2"));
        assertEquals(new BigDecimal(1000), a1.amount);
        assertEquals(new BigDecimal(1000), a2.amount);
    }

    @Test
    void testTransferRejected() {
        BankServer server = new BankServer();
        assertTrue(server.requestTransferMoney(1, 2, new BigDecimal(100)));

        ArrayList<Transaction> ts = server.getAccountTransactions(1);
        assertEquals(1, ts.size());

        Transaction t1 = ts.get(0);
        assertEquals(Transaction.Status.WAIT_CONFIRMATION, t1.status);

        DBConnector db = new DBConnector();
        Account a1 = db.selectAccount(db.makePreparedQuery("select * from ACCOUNTS where id = 1"));
        Account a2 = db.selectAccount(db.makePreparedQuery("select * from ACCOUNTS where id = 2"));
        assertEquals(new BigDecimal(1000), a1.amount);
        assertEquals(new BigDecimal(1000), a2.amount);

        assertTrue(server.rejectTransaction(t1.id));
        assertFalse(server.confirmTransaction(t1.id));
        assertFalse(server.rejectTransaction(t1.id));

        ts = server.getAccountTransactions(1);
        assertEquals(1, ts.size());

        t1 = ts.get(0);
        assertEquals(Transaction.Status.REJECTED, t1.status);

        a1 = db.selectAccount(db.makePreparedQuery("select * from ACCOUNTS where id = 1"));
        a2 = db.selectAccount(db.makePreparedQuery("select * from ACCOUNTS where id = 2"));
        assertEquals(new BigDecimal(1000), a1.amount);
        assertEquals(new BigDecimal(1000), a2.amount);
    }

}
