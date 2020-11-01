package Client;

import MySerdes.ValueSerde;
import Structure.EventVal;
import Structure.SubscribeVal;
import org.apache.kafka.clients.consumer.ConsumerRecord;
import org.apache.kafka.clients.consumer.ConsumerRecords;
import org.apache.kafka.clients.consumer.KafkaConsumer;
import org.apache.kafka.clients.producer.KafkaProducer;
import org.apache.kafka.clients.producer.ProducerRecord;
import org.apache.kafka.common.errors.WakeupException;
import utils.InfluxdbUtil;

import java.io.*;
import java.util.Arrays;
import java.util.Properties;

public class Consumer {
	
	
	public static void main(String[] args) {
        if(args.length < 2){
            System.out.println("Usage: Consumer -matchconfigfile -influxdbconfigfile ");
            System.exit(1);
        }

        String config_filename = "";
        String influx_filename = "";
        try{
            config_filename = args[0];
            influx_filename = args[1];
        } catch (Throwable e){
            System.out.println("Usage: Consumer -matchconfigfile -influxdbconfigfile ");
            e.printStackTrace();
            System.exit(1);
        }


		// read consumer config file
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
		String clientId = properties.getProperty("clientId");
		String securityProto = properties.getProperty("security.protocol");
		String saslName = properties.getProperty("sasl.kerberos.service.name");

		// define the variable of kafka connection prop
		Properties props = new Properties();
		props.put("bootstrap.servers", KafkaServer);
		props.put("group.id", "Consumer");
		props.put("key.deserializer", "org.apache.kafka.common.serialization.StringDeserializer");
		props.put("value.deserializer", ValueSerde.EventValDeserde.class.getName());
		if(securityProto != null && saslName != null){
			props.put("security.protocol", securityProto);
			props.put("sasl.kerberos.service.name", saslName);
		} else {
			System.out.println("WARNING: no authentication configuration");
//			System.exit(1);
		}

		// create consumer
		KafkaConsumer<String, EventVal> consumer = new KafkaConsumer<>(props);
		// consumer subscribe
		consumer.subscribe(Arrays.asList(clientId));
		try{
			sendSubscribe(properties);
		} catch (Throwable e) {
			System.out.println("Sent subscribe error\n");
			System.exit(1);
		}
		InfluxdbUtil influx = InfluxdbUtil.setUp(influx_filename,"client-test");

		//TopicPartition topicPartition = new TopicPartition("", 0);

        // attach shutdown handler to catch control-c
        Runtime.getRuntime().addShutdownHook(new Thread("consumer-shutdown-hook") {
            @Override
            public void run() {
                consumer.wakeup();
                //latch.countDown();
            }
        });

//		BufferedWriter bw = null;
		try {
//			File file = new File("src/main/resources/Stream-data/rcv-time.txt");
//			FileWriter fw = new FileWriter(file, true);
//			bw = new BufferedWriter(fw);

			long tmpArriveTime = 0;
			double aver_delay_time = 0;
			int x = 0;
			// loop to poll events
			while(true) {
				ConsumerRecords<String, EventVal> records = consumer.poll(70);

				/** ATTENTION: not accurate rcv time, for accurate, JMX shoudl be used**/
				tmpArriveTime = System.currentTimeMillis();
				// traverse all records
				for(ConsumerRecord<String, EventVal> record : records) {
					// decode to get EventVal object
					EventVal eVal = record.value();
					eVal.EventGetTime = tmpArriveTime;
					long tmpDelayTime = tmpArriveTime - eVal.EventProduceTime;

					//save rcv_time
					String s = eVal.StockId + " "
						 + eVal.EventArriveTime + " "
						 + eVal.EventMatchTime + " "
						 + eVal.EventGetTime + " "
					         + tmpDelayTime;

//					bw.write(s + "\n");

					influx.consumerInsert(eVal);

				}
			}
		} catch (WakeupException e) {

		} catch (Throwable e) {
			System.exit(1);
		} finally {
//			try {
//				bw.close();
//			}catch (Exception e){
//				e.printStackTrace();
//			}
			consumer.close();
        }
		System.exit(0);
	}

	//client's send subscribe func
	public static int sendSubscribe(Properties properties) {
		String securityProto = properties.getProperty("security.protocol");
		String saslName = properties.getProperty("sasl.kerberos.service.name");

		Properties Props =  new Properties();
		Props.put("bootstrap.servers", properties.getProperty("KafkaServer"));
		Props.put("key.serializer", "org.apache.kafka.common.serialization.StringSerializer");
		Props.put("value.serializer", ValueSerde.SubValSerde.class.getName());
		if(securityProto != null && saslName != null){
			Props.put("security.protocol", securityProto);
			Props.put("sasl.kerberos.service.name", saslName);
		} else {
			System.out.println("WARNING: no authentication configuration");
		}

		KafkaProducer<String, SubscribeVal> producer = new KafkaProducer<>(Props);

		String attrs[] = properties.getProperty("attributes").split(",");
		System.out.println(attrs[0]);
		System.out.println(attrs[1]);
		System.out.println(attrs[2]);


		SubscribeVal sVal = new SubscribeVal(attrs.length);
		sVal.SubId = properties.getProperty("clientId");
//		sVal.sub_num_id = properties.getProperty("Client" + "clientId");
		sVal.StockId = Integer.parseInt(properties.getProperty("stockId"));
		for(int j = 0; j < sVal.AttributeNum; j++){
			String nums_str[] = attrs[j].split(" ");
			sVal.subVals.get(j).attributeId = Integer.parseInt(nums_str[0]);
			sVal.subVals.get(j).min_val = Double.parseDouble(nums_str[1]);
			sVal.subVals.get(j).max_val = Double.parseDouble(nums_str[2]);
		}
		//Record
		ProducerRecord<String, SubscribeVal> record = new ProducerRecord<>("Sub", sVal);
		//send
		try {
			System.out.println(record);
			producer.send(record).get();
			//System.err.println("Producer Send " + i + " Success!");
			//Thread.sleep(stime[st]);
		} catch (Exception e) {
			e.printStackTrace();
			return 1;
		}

		producer.close();

		return 0;
	}
}
