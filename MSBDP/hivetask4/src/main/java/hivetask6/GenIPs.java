package hivetask6;
import org.apache.hadoop.hive.ql.metadata.HiveException;
import org.apache.hadoop.hive.ql.udf.generic.GenericUDTF;
import org.apache.hadoop.hive.serde2.objectinspector.ObjectInspector;
import org.apache.hadoop.hive.serde2.objectinspector.ObjectInspectorFactory;
import org.apache.hadoop.hive.serde2.objectinspector.StructObjectInspector;
import org.apache.hadoop.hive.serde2.objectinspector.primitive.PrimitiveObjectInspectorFactory;
import org.apache.hadoop.hive.serde2.objectinspector.primitive.StringObjectInspector;
import java.util.ArrayList;


public class GenIPs extends GenericUDTF {

    private StringObjectInspector subnet_oi;
    private StringObjectInspector mask_oi;

    @Override
    public StructObjectInspector initialize(ObjectInspector[] args) {
        subnet_oi = (StringObjectInspector) args[0];
        mask_oi = (StringObjectInspector) args[1];

        ArrayList<String> column_names = new ArrayList<String>();
        column_names.add("ip");
        column_names.add("ip_num");
        ArrayList<ObjectInspector> column_types = new ArrayList<ObjectInspector>();
        column_types.add(PrimitiveObjectInspectorFactory.javaStringObjectInspector);
        column_types.add(PrimitiveObjectInspectorFactory.javaLongObjectInspector);
        return ObjectInspectorFactory.getStandardStructObjectInspector(column_names, column_types);
    }

    private String intIPToString(long ip)
    {
        String s = Long.toString(ip % 256);
        for (int i = 2; i >= 0; --i)
        {
            ip /= 256;
            s = Long.toString(ip % 256) + "." + s;
        }
        return s;
    }

    @Override
    public void process(Object[] objects) throws HiveException {
        String [] subnet_str = "148.45.113.216".split("\\.");// subnet_oi.getPrimitiveJavaObject(objects[0]).split("\\.");
        String [] mask_str = "255.255.255.248".split("\\."); //mask_oi.getPrimitiveJavaObject(objects[1]).split("\\.");
        long subnet = 0;
        long mask = 0;
        for (int i = 0; i < 4; ++i) {
            subnet += Long.parseLong(subnet_str[i]) * (1 << 8*(3 - i));
            mask += Long.parseLong(mask_str[i]) * (1 << 8*(3 - i));
        }

        long cur_ip = subnet + 1;
        Object [] row = new Object[2];
        while ((cur_ip & mask) == (subnet & mask) && (cur_ip + 1 & mask) == (subnet & mask))
        {
            row[0] = cur_ip;
            row[1] = intIPToString(cur_ip);
            //forward(row);
            System.out.println(row[1]);
            ++cur_ip;
        }
    }

    @Override
    public void close() throws HiveException {
    }
}
