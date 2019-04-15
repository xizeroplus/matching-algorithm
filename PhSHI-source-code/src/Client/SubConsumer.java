package Client;

import MySerdes.ValueSerde;
import Structure.EventVal;
import org.apache.kafka.clients.consumer.ConsumerRecord;
import org.apache.kafka.clients.consumer.ConsumerRecords;
import org.apache.kafka.clients.consumer.KafkaConsumer;
import org.apache.kafka.common.errors.WakeupException;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.util.Arrays;
import java.util.Properties;

public class SubConsumer {
	
	
	public static void main(String[] args) {
		Properties props = new Properties();
		props.put("bootstrap.servers", "192.168.101.15:9092,192.168.101.12:9092,192.168.101.28:9092");
		props.put("group.id", "Consumer");
		props.put("key.deserializer", "org.apache.kafka.common.serialization.StringDeserializer");
		props.put("value.deserializer", ValueSerde.EventValDeserde.class.getName());
		
		KafkaConsumer<String, EventVal> consumer = new KafkaConsumer<>(props);
		
		/*
		Scanner input = new Scanner(System.in);
		String name;
		name = input.next();
		input.close();
		*/
		consumer.subscribe(Arrays.asList("TestClient"));
		
		//TopicPartition topicPartition = new TopicPartition("", 0);

        // attach shutdown handler to catch control-c
        Runtime.getRuntime().addShutdownHook(new Thread("consumer-shutdown-hook") {
            @Override
            public void run() {
                consumer.wakeup();
                //latch.countDown();
            }
        });
		BufferedWriter bw = null;
		//BufferedWriter bw1 = null;
		try {
			//read Record

			//File file1 = new File("filepath");
			File file = new File("/home/ubuntu/Stream-data/rcv-time-1.txt");

			FileWriter fw = new FileWriter(file, true);
			//FileWriter fw1 = new FileWriter(file1, true);

			bw = new BufferedWriter(fw);
			//bw1 = new BufferedWriter(fw1);
			long tmptime = 0;
			double aver_rcv_time = 0;
			int x = 0;
			while(true) {
				ConsumerRecords<String, EventVal> records = consumer.poll(70);
				// move offset to begin
				//consumer.seekToBeginning(Collections.singleton(topicPartition));
				//consumer.seek(topicPartition, 4);
				tmptime = System.currentTimeMillis();
				for(ConsumerRecord<String, EventVal> record : records) {

					EventVal eVal = record.value();
					eVal.EventGetTime = tmptime - eVal.EventStartSendTime;
					long tmp = tmptime - eVal.EventProduceTime;

					//save rcv_time
					String s = String.valueOf(eVal.StockId) + " " 
						 + String.valueOf(eVal.EventArriveTime) + " "
						 + String.valueOf(eVal.EventMatchTime / 1000000.0) + " "
						 + String.valueOf(eVal.EventGetTime) + " "
					         + String.valueOf(tmp);
					
					bw.write(s + "\n");

					/*save event
					int n = eVal.AttributeNum;
					s = "";
					for(int i = 0; i < n; i++) {
						s = s + String.valueOf(eVal.eventVals[i].attributeId) + " " + eVal.eventVals[i].val + " ";
						//System.out.println("attribute id: " + eVal.eventVals[i].attributeId + " value: " + eVal.eventVals[i].val);
					}
					bw1.write(s + "\n");
					*/

					aver_rcv_time += tmp;
					x++;
					if(x % 1000 == 0){
						System.out.println("1000rcv aver_ time: " + (aver_rcv_time / 1000.0));
						aver_rcv_time = 0;
						x = 0;
					}
				}
			}
		} catch (WakeupException e) {

		} catch (Throwable e) {
			System.exit(1);
		} finally {
			try {
				bw.close();
				//bw1.close();
			}catch (Exception e){
				e.printStackTrace();
			}
			consumer.close();
        }
		System.exit(0);
	}
}
