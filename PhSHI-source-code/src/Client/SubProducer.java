package Client;

import MySerdes.ValueSerde;
import Structure.SubscribeVal;
import org.apache.kafka.clients.producer.KafkaProducer;
import org.apache.kafka.clients.producer.ProducerRecord;

import java.io.File;
import java.util.Properties;
import java.util.Scanner;

public class SubProducer {
    //read file
    public static void main(String[] args) {
        Scanner input = new Scanner(System.in);
        Properties Props =  new Properties();
        Props.put("bootstrap.servers", "192.168.101.15:9092,192.168.101.12:9092,192.168.101.28:9092");
        Props.put("key.serializer", "org.apache.kafka.common.serialization.StringSerializer");
        Props.put("value.serializer", ValueSerde.SubValSerde.class.getName());

        KafkaProducer<String, SubscribeVal> producer = new KafkaProducer<>(Props);
        Scanner s = null;
        try{
            File file = new File("resources/");
            s = new Scanner(file);
            s.nextInt();
            s.nextInt();
            s.nextInt();
        }catch (Throwable e){
            System.exit(1);
        }
        System.out.println("input sub num: ");
        int num = input.nextInt();
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
            ProducerRecord<String, SubscribeVal> record = new ProducerRecord<>("NewSub", sVal);
            //send
            try {
                producer.send(record).get();
                System.err.println("Producer Send " + i + " Success!");
                Thread.sleep(10);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        producer.close();
        s.close();
    }
}
