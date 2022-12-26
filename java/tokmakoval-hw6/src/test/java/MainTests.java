import org.junit.jupiter.api.Test;

import java.math.BigDecimal;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.time.Duration;

import static org.junit.jupiter.api.Assertions.*;

class MainTests {
    static private final int INIT_TIMEOUT_SECS = 5;
    static private final int ACTIONS_TIMEOUT_SECS = 120;

    @Test
    void testRandomUserActions() throws SQLException
    {
        assertTimeout(Duration.ofSeconds(INIT_TIMEOUT_SECS), () -> Main.initDb());
        assertTimeout(Duration.ofSeconds(ACTIONS_TIMEOUT_SECS), () -> Main.randomUserActions(20));

        DBConnector db = new DBConnector();
        DBConnector.Query q = db.makePreparedQuery("select sum(amount) from ACCOUNTS");
        ResultSet res = q.st.executeQuery();
        res.next();
        BigDecimal expextedSum = new BigDecimal(20 * ClientThread.AMOUNT);
        assertEquals(expextedSum, res.getBigDecimal(1));
    }
}
