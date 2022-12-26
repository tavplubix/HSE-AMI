import java.util.Date;

/**
 * Хранит данные о сессии
 */
public class Session {

    Session(String token, Date begin, Date expire, int uid) {
        this.token = token;
        this.begin = begin;
        this.expire = expire;
        this.uid = uid;
    }

    String token;
    Date begin;
    Date expire;
    int uid;
}
