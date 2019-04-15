package EventSender;

import MySerdes.ValueSerde;
import Structure.EventVal;
import org.apache.kafka.clients.producer.KafkaProducer;
import org.apache.kafka.clients.producer.ProducerRecord;

import java.io.*;
import java.util.Properties;
import java.util.Scanner;

public class EventProducer {
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
		String KafkaServer = properties.getProperty("KafkaServer");
		String EventFile = properties.getProperty("EventFile");

		Properties Props =  new Properties();
		Props.put("bootstrap.servers", KafkaServer);
		Props.put("key.serializer", "org.apache.kafka.common.serialization.StringSerializer");
		Props.put("value.serializer", ValueSerde.EventValSerde.class.getName());
		//creat producer
		KafkaProducer<String, EventVal> producer = new KafkaProducer<>(Props);
		Scanner input = new Scanner(System.in);
		int num;
		int stock_id;
		int max_attri_num;
		Scanner s = null;
		try{
			File file = new File(EventFile);
			s = new Scanner(file);
		}catch (Throwable e){
			System.exit(1);
		}
		s.nextInt();
		stock_id = s.nextInt();
		max_attri_num = s.nextInt();

		System.out.println("input event num: ");
		num = input.nextInt();
		input.nextLine();
		System.out.println("input event attribute num: ");
		int attribute_num = input.nextInt();
		if(attribute_num > max_attri_num){
			System.err.println("Over the max attribute num!");
			System.exit(1);
		}
		EventVal[] eVal = new EventVal[num];
		for(int i = 0; i < num; i++) {
			eVal[i] = new EventVal(attribute_num, stock_id);
			for(int j = 0; j < max_attri_num; j++) {
				if(j >= attribute_num){
					s.nextInt();
					s.nextDouble();
					continue;
				}
				eVal[i].eventVals[j].attributeId = s.nextInt();
				eVal[i].eventVals[j].val = s.nextDouble();
			}
		}
		//long time = System.currentTimeMillis();
		//for(int i=0;i<num;i++){
		int j = 0;
		//while((System.currentTimeMillis() - time) < 22*60*1000){
		while(j <  num){
			int i = j % num;
			eVal[i].EventProduceTime = System.currentTimeMillis();
			// creat Record
			ProducerRecord<String, EventVal> record = new ProducerRecord<>("Event", eVal[i]);
			//send
			try {
				producer.send(record);
				Thread.sleep(5);
			} catch (Exception e) {
				e.printStackTrace();
			}
			j++;
			//System.err.println("Producer Send " + i + " Success!");
		}
		producer.close();
		input.close();
	}
}
