import java.util.ArrayList;
import java.util.Date;
import java.util.Random;

/**
 * Управляет сессиями и проверяет права доступа
 */
class Checker {
    enum Rights {
        NONE, FULL
    }

    static class A2U {
        A2U(int uid, int aid, int rights) {
            this.uid = uid;
            this.aid = aid;
            this.rights = Rights.values()[rights];
        }

        int uid;
        int aid;
        Rights rights;
    }

    private static final String UPPER = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    private static final String LOWER = UPPER.toLowerCase();
    private static final String DIGITS = "0123456789";
    private static final String ALPHANUM = UPPER + LOWER + DIGITS;
    private static final int TOKEN_LEN = 16;
    private static final int EXPIRE_IN_SECS = 60 * 60 * 1000;

    private final DBConnector db = new DBConnector();
    private final Random random = new Random();

    private final DBConnector.Query deleteSessionByToken = db.makePreparedQuery("delete from SESSIONS where token = ?");
    private final DBConnector.Query selectSessionByToken = db.makePreparedQuery(
            "select token, begin, expire, \"uid\" " +
                    "from SESSIONS where token = ?");
    private final DBConnector.Query selectUserByLogin = db.makePreparedQuery("select id, login, password, name " +
            "from USERS where login = ?");
    private final DBConnector.Query insertSession = db.makePreparedQuery("insert into SESSIONS values (?, ?, ?, ?)");
    private final DBConnector.Query selectA2U = db.makePreparedQuery("select \"uid\", aid, rights from A2U where \"uid\" = ? and aid = ?");

    /**
     * Открывает сессию для пользователя
     */
    String beginSession(String login, String password) {
        User u = db.selectUser(selectUserByLogin.setArg1(login));
        if (!password.equals(u.password))
            throw new RuntimeException("incorrect password");
        StringBuilder buf = new StringBuilder(TOKEN_LEN);
        for (int i = 0; i < TOKEN_LEN; ++i)
            buf.append(ALPHANUM.charAt(random.nextInt(ALPHANUM.length())));
        String token = buf.toString();
        Date begin = new Date();
        Date expire = new Date(begin.getTime() + EXPIRE_IN_SECS);
        db.executeOrDie(insertSession.setArg(1, token).setArg(2, begin).setArg(3, expire).setArg(4, u.id));
        return token;
    }

    /**
     * Закрывает сессию
     *
     * @param token токен сессии
     */
    void endSession(String token) {
        db.executeOrDie(deleteSessionByToken.setArg1(token));
    }


    /**
     * Возвращает номер пользователя по токену сессии, если токен валидный
     *
     * @param token токен сессии
     * @return номер пользователя или null
     */
    Integer getUidIfSessionValid(String token) {
        ArrayList<Session> ss = db.selectSessions(selectSessionByToken.setArg1(token));
        if (ss.size() != 1) return null;
        Session s = ss.get(0);
        return s.expire.after(new Date()) ? s.uid : null;
    }

    /**
     * Проверяет, есть ли у пользователя необходимые права доступа к счёту
     *
     * @param uid   номер пользователя
     * @param aid   номер счёта
     * @param right права
     * @return true, если есть
     */
    boolean hasAccessToAccount(int uid, int aid, Rights right) {
        ArrayList<A2U> a2u = db.selectA2Us(selectA2U.setArg(1, uid).setArg(2, aid));
        return a2u.size() == 1 && a2u.get(0).rights == right;
    }

}
