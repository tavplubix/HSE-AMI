import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;
import static org.junit.jupiter.api.Assertions.*;

public class CheckerTests {

    @BeforeAll
    static void initDb()
    {
        Main.initDb();
    }

    @Test
    void testSessions()
    {
        Checker checker = new Checker();
        assertDoesNotThrow(() -> checker.endSession("invalid_token"));
        assertThrows(RuntimeException.class, () -> checker.beginSession("invalid", "invalid"));
        assertThrows(RuntimeException.class, () -> checker.beginSession("user1", "invalid"));
        assertDoesNotThrow(() -> checker.endSession(checker.beginSession("user1", "qwerty1")));
    }

    @Test
    void testTokenToUid()
    {
        Checker checker = new Checker();
        String token = checker.beginSession("user1", "qwerty1");
        assertEquals(1, checker.getUidIfSessionValid(token));
        checker.endSession(token);
        assertNull(checker.getUidIfSessionValid(token));
    }

    @Test
    void testAccess()
    {
        Checker checker = new Checker();
        String token = checker.beginSession("user1", "qwerty1");
        assertTrue(checker.hasAccessToAccount(1, 1, Checker.Rights.FULL));
        assertTrue(checker.hasAccessToAccount(1, 1001, Checker.Rights.FULL));
        assertFalse(checker.hasAccessToAccount(2, 1, Checker.Rights.FULL));
        assertFalse(checker.hasAccessToAccount(2, 1001, Checker.Rights.FULL));
    }
}
