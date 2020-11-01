package EventSender;

import MySerdes.ValueSerde;
import Structure.EventVal;
import org.apache.kafka.clients.producer.KafkaProducer;
import org.apache.kafka.clients.producer.ProducerRecord;

import java.io.*;
import java.util.Properties;
import java.util.Scanner;

public class EventProducer {

	private int period;

	public static void main(String[] args) {
		if(args.length < 4){
			System.out.println("Usage: EventProducer -configfile -eventnum -attrnum -max-frequency");
			System.exit(1);
		}

		String config_filename = "";
		int num = 0, attribute_num = 0;
		int period = 0;

		try{
			config_filename = args[0];
			num = Integer.parseInt(args[1]);
			attribute_num = Integer.parseInt(args[2]);
			period = Integer.parseInt(args[3]);
		} catch (Throwable e){
			System.out.println("Usage: EventProducer -configfile -eventnum -attrnum -frequency");
			e.printStackTrace();
			System.exit(1);
		}
		if(num==0 || attribute_num==0 || period==0) {
			System.out.println("need non-zero number\n");
			System.exit(2);
		}

	    // read config file
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
		String KafkaServer = properties.getProperty("KafkaServer");
		String EventFile = properties.getProperty("EventFile");

        //configure producer props
		Properties Props =  new Properties();
		Props.put("bootstrap.servers", KafkaServer);
		Props.put("key.serializer", "org.apache.kafka.common.serialization.StringSerializer");
		Props.put("value.serializer", ValueSerde.EventValSerde.class.getName());
		//creat producer
		KafkaProducer<String, EventVal> producer = new KafkaProducer<>(Props);
		// console input events props
//		Scanner input = new Scanner(System.in);

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

//		System.out.println("input event num: ");
//		num = input.nextInt();
//		input.nextLine();
//		System.out.println("input event attribute num: ");
//		int attribute_num = input.nextInt();
		if(attribute_num > max_attri_num){
			System.err.println("Over the max attribute num!");
			System.exit(1);
		}

		// read and send events
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

		int[] frequencies = new int[]{400,200,100,90,80,70,60,50,40,30,
				40,50,60,70,80};

		int freLen = frequencies.length;
		int fre_idx = 0;
		int curCount = 0;
		//while((System.currentTimeMillis() - time) < 22*60*1000){
		while(j <  num ){
			int i = j % num;
			eVal[i].EventProduceTime = System.currentTimeMillis();

			// creat Record
			ProducerRecord<String, EventVal> record = new ProducerRecord<>("Event", eVal[i]);
			//send
			try {
				producer.send(record);

				Thread.sleep(frequencies[fre_idx]);
				System.out.println("event "+j+" with frequency "+frequencies[fre_idx]);

			} catch (Exception e) {
				e.printStackTrace();
			}
			j++;
			curCount++;
			if( curCount * frequencies[fre_idx]  >= period ){
				if(fre_idx == freLen-1){
					break;
				}
				fre_idx = (fre_idx + 1) % freLen;
				curCount = 0;
			}
		}
		producer.close();
//		input.close();
	}
}
