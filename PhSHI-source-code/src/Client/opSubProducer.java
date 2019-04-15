package Client;

import MySerdes.ValueSerde;
import Structure.SubscribeVal;
import org.apache.kafka.clients.producer.KafkaProducer;
import org.apache.kafka.clients.producer.ProducerRecord;

import java.io.*;
import java.util.ArrayList;
import java.util.Properties;
import java.util.Scanner;

public class opSubProducer {

    private static int MAX_ATTRIBUITE_NUM;
    private static ArrayList<Pivot> Pivot_Attri = new ArrayList<>();

    static public int compare(Pivot o1, Pivot o2) {
        if ( o1.num > o2.num ) {
            return 1;
        } else {
            return -1;
        }
    }

    public static void main(String[] args) {
        Properties properties = new Properties();
        try {
            InputStream inputStream = new FileInputStream(new File("resources/config.properties"));
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
        Props.put("bootstrap.servers", "localhost:9092");
        Props.put("key.serializer", "org.apache.kafka.common.serialization.StringSerializer");
        Props.put("value.serializer", ValueSerde.SubValSerde.class.getName());

        KafkaProducer<String, SubscribeVal> producer = new KafkaProducer<>(Props);

        Scanner s = null;
        try{
            File file = new File(SubFile);
            s = new Scanner(file);
            s.nextInt();
            MAX_ATTRIBUITE_NUM = s.nextInt();
            s.nextInt();
        }catch (Throwable e){
            System.err.println("file read failed!");
            System.exit(1);
        }

        for(int i = 0; i < MAX_ATTRIBUITE_NUM; i++)Pivot_Attri.add(new Pivot(i));
	while(true){
        System.out.println("input sub num: ");
        int num = input.nextInt();
	if(num==0)break;
        for(int i = 0; i < num; i++) {
            String SubId = s.next();
            int StockId = s.nextInt();
            int AttributeNum = s.nextInt();
            int max = MAX_ATTRIBUITE_NUM;
            SubscribeVal sVal = new SubscribeVal(AttributeNum);
            sVal.SubId = SubId;
            sVal.StockId = StockId;
            for(int j = 0; j < sVal.AttributeNum; j++){
                sVal.subVals.get(j).attributeId = s.nextInt();
                sVal.subVals.get(j).min_val = s.nextDouble();
                sVal.subVals.get(j).max_val = s.nextDouble();
                for( int w = 0; w < MAX_ATTRIBUITE_NUM; w++ )
                    if( Pivot_Attri.get(w).Attri_id == sVal.subVals.get(j).attributeId ){
                        Pivot_Attri.get(w).num++;
                        break;
                    }
            }
            Pivot_Attri.sort(opSubProducer::compare);
            for(int j = 0; j < sVal.AttributeNum; j++){
                int tmp = 0;
                int m = 0;
                while( sVal.subVals.get(j).attributeId != Pivot_Attri.get( m++ ).Attri_id || Pivot_Attri.get( m++ ).num == 0)
                    if( ++tmp >= MAX_ATTRIBUITE_NUM - 1 )break;
                max = max > tmp ? tmp : max;
            }
            sVal.Pivot_Attri_Id = Pivot_Attri.get(max).Attri_id;
            //Record
            ProducerRecord<String, SubscribeVal> record = new ProducerRecord<>("Sub", sVal);
            //send
            try {
                producer.send(record).get();
                //System.err.println("Producer Send " + i + " Success!");
            } catch (Exception e) {
                e.printStackTrace();
            }
        }}
        producer.close();
        s.close();
    }
    static class Pivot{
        public int Attri_id;
        public int num;

        public Pivot(int i){
            this.Attri_id = i;
            this.num = 0;
        }
    }
}
