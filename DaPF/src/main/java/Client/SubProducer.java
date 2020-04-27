package Client;

import MySerdes.ValueSerde;
import Structure.SubscribeVal;
import org.apache.kafka.clients.producer.KafkaProducer;
import org.apache.kafka.clients.producer.ProducerRecord;
import scala.Int;

import java.io.*;
import java.util.Properties;
import java.util.Scanner;

public class SubProducer {
    //read file
    public static void main(String[] args) {
        if(args.length < 2){
            System.out.println("Usage: subProducer -configfile -subnum");
            System.exit(1);
        }

        String config_filename = "";
        int num = 0;
        try{
            config_filename = args[0];
            num = Integer.parseInt(args[1]);
        } catch (Throwable e){
            System.out.println("Usage: subProducer -configfile -subnum");
            e.printStackTrace();
            System.exit(1);
        }
        if(num==0) {
            System.out.println("need non-zero number\n");
            System.exit(2);
        }

        Properties properties = new Properties();
        try {
            InputStream inputStream = new FileInputStream(new File(config_filename));
            properties.load(inputStream);
        } catch (FileNotFoundException e) {
            System.err.println("properties file open failed!");
            e.printStackTrace();
        } catch (IOException e) {
            System.err.println("properties file read failed");
            e.printStackTrace();
        }
        String SubFile = properties.getProperty("SubFile");
        String KafkaServer = properties.getProperty("KafkaServer");

        Scanner input = new Scanner(System.in);
        Properties Props =  new Properties();
        Props.put("bootstrap.servers", KafkaServer);
        Props.put("key.serializer", "org.apache.kafka.common.serialization.StringSerializer");
        Props.put("value.serializer", ValueSerde.SubValSerde.class.getName());

        KafkaProducer<String, SubscribeVal> producer = new KafkaProducer<>(Props);
        Scanner s = null;
        try{
            File file = new File(SubFile);
            s = new Scanner(file);
            s.nextInt();
            s.nextInt();
            s.nextInt();
        }catch (Throwable e){
            System.err.println("file read failed");
            System.exit(1);
        }

        long beginTime = System.currentTimeMillis();
        for(int i = 0; i < num; i++) {
            String SubId = s.next();
            int StockId = s.nextInt();
            int AttributeNum = s.nextInt();
            SubscribeVal sVal = new SubscribeVal(AttributeNum);
            sVal.SubId = SubId;

            sVal.StockId = StockId;
            for(int j = 0; j < sVal.AttributeNum; j++){
                sVal.subVals.get(j).attributeId = s.nextInt();
                sVal.subVals.get(j).min_val = s.nextDouble();
                sVal.subVals.get(j).max_val = s.nextDouble();
            }
            //Record
            ProducerRecord<String, SubscribeVal> record = new ProducerRecord<>("Sub", sVal);
            //send
            try {
                producer.send(record).get();
                Thread.sleep(1);
                System.out.println("Record " + i);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        long endTime = System.currentTimeMillis();
        System.out.println("Total used time: " + (endTime-beginTime)/60000.0 + "min");


        producer.close();
        s.close();
    }
}
