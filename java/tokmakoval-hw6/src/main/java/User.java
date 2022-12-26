
/**
 * Хранит данные о пользователе
 */
public class User {

    User(int id, String login, String password, String name) {
        this.id = id;
        this.login = login;
        this.password = password;
        this.name = name;
    }

    int id;
    String login;
    String password;
    String name;
}
