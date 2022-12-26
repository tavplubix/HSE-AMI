package hivetask6;

import org.apache.hadoop.hive.ql.metadata.HiveException;

public class Main {
    public static void main(String[] args) throws HiveException {

        GenIPs tmp = new GenIPs();
        tmp.process(new Object[3]);

    }
}
