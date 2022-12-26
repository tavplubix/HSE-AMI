import java.math.BigDecimal;
import java.sql.*;
import java.util.Date;
import java.util.ArrayList;
import java.util.logging.Level;

import oracle.jdbc.OracleDriver;

/**
 * Предоставляет доступ к бд, конструирует объекты классов из ResultSet
 */
public class DBConnector {
    static {
        try {
            DriverManager.registerDriver(new OracleDriver());
        } catch (SQLException e) {
            System.exit(1);
        }
    }

    /**
     * Обёртка над PreparedStatement
     */
    class Query {
        Query setArg1(int val) {
            return setArg(1, val);
        }

        Query setArg1(String val) {
            return setArg(1, val);
        }

        Query setArg(int idx, int val) {
            try {
                st.setInt(idx, val);
                return this;
            } catch (SQLException e) {
                handleSQLException(e);
            }
            throw new RuntimeException("unreachable");
        }

        Query setArg(int idx, String val) {
            try {
                st.setString(idx, val);
                return this;
            } catch (SQLException e) {
                handleSQLException(e);
            }
            throw new RuntimeException("unreachable");
        }

        Query setArg(int idx, Date val) {
            try {
                java.sql.Timestamp d = new java.sql.Timestamp(val.getTime());
                st.setTimestamp(idx, d);
                return this;
            } catch (SQLException e) {
                handleSQLException(e);
            }
            throw new RuntimeException("unreachable");
        }

        Query setArg(int idx, BigDecimal val) {
            try {
                st.setBigDecimal(idx, val);
                return this;
            } catch (SQLException e) {
                handleSQLException(e);
            }
            throw new RuntimeException("unreachable");
        }

        PreparedStatement st;
    }

    private static final String CONNECTION_STRING = "jdbc:oracle:thin:@localhost:1521:XE";
    private static final String USER = "system";
    private static final String PASSWORD = "oracle";

    private Connection db;


    DBConnector() {
        try {
            db = DriverManager.getConnection(CONNECTION_STRING, USER, PASSWORD);
        } catch (SQLException e) {
            handleSQLException(e);
        }
    }

    Query makePreparedQuery(String query) {
        try {
            Query q = new Query();
            q.st = db.prepareStatement(query);
            return q;
        } catch (SQLException e) {
            handleSQLException(e);
        }
        throw new RuntimeException("unreachable");
    }

    Query makeCallable(String query) {
        try {
            Query q = new Query();
            q.st = db.prepareCall(query);
            return q;
        } catch (SQLException e) {
            handleSQLException(e);
        }
        throw new RuntimeException("unreachable");
    }

    /**
     * Выполняет запрос, получающий список сессий
     *
     * @param query запрос
     * @return список сессий
     */
    ArrayList<Session> selectSessions(Query query) {
        ArrayList<Session> s = new ArrayList<>();
        try {
            ResultSet res = query.st.executeQuery();
            while (res.next())
                s.add(sessionFromResultSet(res));
        } catch (SQLException e) {
            handleSQLException(e);
        }
        return s;
    }

    /**
     * Выполняет запрос, получающий ровно одну сессию
     *
     * @param query запрос
     * @return сессия
     */
    Session selectSession(Query query) {
        ArrayList<Session> s = selectSessions(query);
        assertUnique(s);
        return s.get(0);
    }

    private Session sessionFromResultSet(ResultSet res) throws SQLException {
        return new Session(res.getString(1),
                res.getTimestamp(2),
                res.getTimestamp(3),
                res.getInt(4));
    }

    /**
     * Выполняет запрос, получающий список полльзователей
     *
     * @param query запрос
     * @return список пользователей
     */
    ArrayList<User> selectUsers(Query query) {
        ArrayList<User> u = new ArrayList<>();
        try {
            ResultSet res = query.st.executeQuery();
            while (res.next())
                u.add(userFromResultSet(res));
        } catch (SQLException e) {
            handleSQLException(e);
        }
        return u;
    }

    /**
     * Выполняет запрос, получающий ровно одного пользователя
     *
     * @param query запрос
     * @return пользователь
     */
    User selectUser(Query query) {
        ArrayList<User> u = selectUsers(query);
        assertUnique(u);
        return u.get(0);
    }

    private User userFromResultSet(ResultSet res) throws SQLException {
        return new User(res.getInt(1),
                res.getString(2),
                res.getString(3),
                res.getString(4));
    }

    /**
     * Выполняет запрос, получающий список счетов
     *
     * @param query запрос
     * @return список счетов
     */
    ArrayList<Account> selectAccounts(Query query) {
        ArrayList<Account> a = new ArrayList<>();
        try {
            ResultSet res = query.st.executeQuery();
            while (res.next())
                a.add(accountFromResultSet(res));
        } catch (SQLException e) {
            handleSQLException(e);
        }
        return a;
    }

    /**
     * Выполняет запрос, получающий ровно один счёт
     *
     * @param query запрос
     * @return счёт
     */
    Account selectAccount(Query query) {
        ArrayList<Account> a = selectAccounts(query);
        assertUnique(a);
        return a.get(0);
    }

    private Account accountFromResultSet(ResultSet res) throws SQLException {
        return new Account(res.getInt(1),
                res.getString(2),
                res.getInt(3),
                res.getBigDecimal(4));
    }

    /**
     * Выполняет запрос, получающий список транзакций
     *
     * @param query запрос
     * @return список транзакций
     */
    ArrayList<Transaction> selectTransactions(Query query) {
        ArrayList<Transaction> a = new ArrayList<>();
        try {
            ResultSet res = query.st.executeQuery();
            while (res.next())
                a.add(transactionFromResultSet(res));
        } catch (SQLException e) {
            handleSQLException(e);
        }
        return a;
    }

    /**
     * Выполняет запрос, получающий ровно одну привязку аккаунта к пользователю
     *
     * @param query запрос
     * @return привязка аккаунта к пользователю
     */
    Checker.A2U selectA2U(Query query) {
        ArrayList<Checker.A2U> a = selectA2Us(query);
        assertUnique(a);
        return a.get(0);
    }

    private Checker.A2U a2uFromResultSet(ResultSet res) throws SQLException {
        return new Checker.A2U(res.getInt(1),
                res.getInt(2),
                res.getInt(3));
    }

    /**
     * Выполняет запрос, получающий список привязок аккаунтов к пользователям
     *
     * @param query запрос
     * @return список привязок аккаунтов к пользователям
     */
    ArrayList<Checker.A2U> selectA2Us(Query query) {
        ArrayList<Checker.A2U> a = new ArrayList<>();
        try {
            ResultSet res = query.st.executeQuery();
            while (res.next())
                a.add(a2uFromResultSet(res));
        } catch (SQLException e) {
            handleSQLException(e);
        }
        return a;
    }

    /**
     * Выполняет запрос, получащий ровно одну транзакцию
     *
     * @param query запрос
     * @return транзакция
     */
    Transaction selectTransaction(Query query) {
        ArrayList<Transaction> t = selectTransactions(query);
        assertUnique(t);
        return t.get(0);
    }


    private Transaction transactionFromResultSet(ResultSet res) throws SQLException {
        return new Transaction(res.getInt(1),
                res.getDate(2),
                res.getInt(3),
                res.getInt(4),
                res.getBigDecimal(5),
                res.getInt(6));
    }

    private <T> void assertUnique(ArrayList<T> result) {
        if (result.isEmpty())
            throw new RuntimeException("empty list of selected objects");
        if (result.size() != 1)
            throw new RuntimeException("selected object is not unique");
    }


    /**
     * Выполняет запрос. Если не получилось - завершает приложение
     *
     * @param query запрос
     */
    void executeOrDie(Query query) {
        try {
            query.st.executeQuery();
        } catch (SQLException e) {
            handleSQLException(e);
        }
    }

    /**
     * Выполняет запрос. Если не получилось - завершает приложение
     *
     * @param sql_query запрос
     */
    void executeOrDie(String sql_query) {
        try {
            Statement st = db.createStatement();
            st.executeQuery(sql_query);
        } catch (SQLException e) {
            handleSQLException(e);
        }
    }

    void handleSQLException(SQLException e) {
        System.out.println("CRITICAL: cannot connect to DB: " + e.getMessage() + "\n");
        e.printStackTrace();
        System.exit(1);
    }

}
