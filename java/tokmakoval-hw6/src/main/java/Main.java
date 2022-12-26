import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;

public class Main {
    public static void main(String[] args) throws InterruptedException {
        initDb();
        randomUserActions(50);
    }

    static void initDb() {
        DBConnector db = new DBConnector();
        dropTables(db);
        createTables(db);
        createProcedures(db);
        insertTestData(db);
    }

    static void randomUserActions(int numberOfThreads) throws InterruptedException {
        Thread[] clients = new Thread[numberOfThreads];

        for (int i = 0; i < numberOfThreads; ++i) {
            clients[i] = new ClientThread(i);
            clients[i].start();
        }

        for (int i = 0; i < numberOfThreads; ++i)
            clients[i].join();
    }

    static void dropTables(DBConnector db) {
        runScriptFromFile(db, "sql/DropAll.sql", false);
    }

    static void createTables(DBConnector db) {
        runScriptFromFile(db, "sql/Create.sql", true);
    }

    static void createProcedures(DBConnector db) {
        runScriptFromFile(db, "sql/Procedures.sql", false);
    }

    static void insertTestData(DBConnector db) {
        runScriptFromFile(db, "sql/InsertTestData.sql", true);
    }

    static void runScriptFromFile(DBConnector db, String path, boolean split) {
        String[] queries = null;
        try {
            String query = new String(Files.readAllBytes(Paths.get(path)), StandardCharsets.UTF_8);
            if (split) {
                queries = query.split(";");
            } else {
                queries = new String[1];
                queries[0] = query;
            }
        } catch (IOException e) {
            System.out.println("CRITICAL: cannot read SQL script: " + e.getMessage());
            System.exit(1);
        }
        for (String query : queries)
            db.executeOrDie(query);
    }

}
