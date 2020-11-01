package utils;

import org.influxdb.InfluxDB;
import org.influxdb.InfluxDBFactory;
import org.influxdb.dto.Point;
import org.influxdb.dto.Point.Builder;
import org.influxdb.dto.Pong;
import org.influxdb.dto.Query;
import org.influxdb.dto.QueryResult;

import java.io.*;
import java.util.HashMap;
import java.util.Map;
import java.util.Properties;

import Structure.EventVal;


public class InfluxdbUtil {
    private static String openurl;
    private static String username;
    private static String password;
    private static String database;
    private static String measurement ;
    private static String measurement2 ;

    private InfluxDB influxDB;

    public InfluxdbUtil(String measurement){
        this.measurement = measurement;
    }
    public InfluxdbUtil(String measurement, String measurement2){
        this.measurement = measurement;
        this.measurement2 = measurement2;
    }

    public static InfluxdbUtil setUp(String config_file, String measurement){
        //创建util对象
        InfluxdbUtil influxDbUtil = new InfluxdbUtil(measurement);
        influxDbUtil.influxDbBuild(config_file);
        return influxDbUtil;
    }
    public static InfluxdbUtil setUp(String config_file, String measurement, String measurement2){
        //创建util对象
        InfluxdbUtil influxDbUtil = new InfluxdbUtil(measurement, measurement2);
        influxDbUtil.influxDbBuild(config_file);
        return influxDbUtil;
    }

    /**连接时序数据库；获得InfluxDB**/
    public InfluxDB  influxDbBuild(String config_file){
        if(influxDB == null){
            // read influxdb config file
            Properties properties = new Properties();
            try {
                InputStream inputStream = new FileInputStream(new File(config_file));
                properties.load(inputStream);
            } catch (FileNotFoundException e) {
                System.err.println("influx properties file open failed!");
                e.printStackTrace();
            } catch (IOException e) {
                System.err.println("influx properties file read failed");
                e.printStackTrace();
            }
            openurl = properties.getProperty("openurl");
            username = properties.getProperty("username");
            password = properties.getProperty("password");
            database = properties.getProperty("database");

            influxDB = InfluxDBFactory.connect(openurl, username, password);

            Pong response = this.influxDB.ping();
            if (response.getVersion().equalsIgnoreCase("unknown")) {
                System.err.println("Error pinging server.");
                return null;
            }
        }
        return influxDB;
    }


    /**
     * 设置数据保存策略
     * defalut 策略名 /database 数据库名/ 30d 数据保存时限30天/ 1  副本个数为1/ 结尾DEFAULT 表示 设为默认的策略
     */
    public void createRetentionPolicy(){
        String command = String.format("CREATE RETENTION POLICY \"%s\" ON \"%s\" DURATION %s REPLICATION %s DEFAULT",
                "defalut", database, "7d", 1);
        this.query(command);
    }

    /**
     * 查询
     * @param command 查询语句
     * @return
     */
    public QueryResult query(String command){
        return influxDB.query(new Query(command, database));
    }

    /**
     * 插入
     * @param eVal 字段
     */
    public void producerInsert(EventVal eVal){
        Builder builder = Point.measurement(measurement);

        Map<String, Object> fields = new HashMap<String,Object>();
        fields.put("EventProduceTime", eVal.EventProduceTime);

        builder.fields(fields);

        influxDB.write(database, "", builder.build());
    }

    /**
     * 插入
     * @param eVal 字段
     */
    public void consumerInsert(EventVal eVal){
        Builder builder = Point.measurement(measurement);

        Map<String, Object> fields = new HashMap<String,Object>();
//        fields.put("curSubEventMatchTime", eVal.EventMatchTime);
        fields.put("EventGetTime", eVal.EventGetTime);
        fields.put("EventProduceTime", eVal.EventProduceTime);
        fields.put("EventTotalMatchTime", eVal.EventMatchTime / 1000000.0);
        fields.put("EventDelay", eVal.EventGetTime-eVal.EventProduceTime);

        builder.fields(fields);

        influxDB.write(database, "", builder.build());
    }

    /**
     * 插入
     * @param EventProduceTime 字段
     */
    public void matcherInsert(long EventProduceTime, Object EventMatchTime,
                              Object EventSendTime, long mintime, long maxtime){
        Builder builder = Point.measurement(measurement);

        Map<String, Object> fields = new HashMap<String,Object>();
        fields.put("EventProduceTime", EventProduceTime);
        fields.put("EventMatchTime", EventMatchTime);
        fields.put("EventSendTime", EventSendTime);
        fields.put("MaxThreadExecTime", maxtime);
        fields.put("MinThreadExecTime", mintime);

        builder.fields(fields);

        influxDB.write(database, "", builder.build());
    }

    /**
     * 插入
     * @param  MaxThreadNum 字段
     */
    public void mactcherStateInsert(int MaxThreadNum,int ExpMatchTime,
                                    double AdjustRatio,double  lastMatchThread,
                                    double avgTime, int WorkingThreadNum){
        Builder builder = Point.measurement(measurement2);

        Map<String, Object> fields = new HashMap<String,Object>();
        fields.put("MaxThreadNum", MaxThreadNum);
        fields.put("NewThreadNum", WorkingThreadNum);
        fields.put("AdjustRatio", AdjustRatio);
        fields.put("ExpMatchTime", ExpMatchTime);
        fields.put("WinAvgTime", avgTime);
        fields.put("LastThreadNum", lastMatchThread);

        builder.fields(fields);

        influxDB.write(database, "", builder.build());
    }



}
