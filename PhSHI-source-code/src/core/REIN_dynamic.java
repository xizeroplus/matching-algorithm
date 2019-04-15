package core;

import MySerdes.ValueSerde;
import Structure.*;
import org.apache.kafka.clients.producer.KafkaProducer;
import org.apache.kafka.clients.producer.ProducerRecord;
import org.apache.kafka.common.serialization.Serdes;
import org.apache.kafka.streams.*;
import org.apache.kafka.streams.kstream.KStream;

import java.io.*;
import java.util.Properties;
import java.util.concurrent.*;

public class REIN_dynamic {
    private final static int TPYE_MATCH = 0;
    private final static int TYPE_SEND = 1;
    private final static int PART = 1000;
    private final static int MAX_VALUE = 1000;
    private final static int MAX_THREAD_Num = 48;
    private final static int MAX_SUB_NUM = 1000001;
    private final static double GROUP_WIDTH = (double)MAX_VALUE / (double)PART;
    private final static int STOCKNUM = 2;
    private final static int ATTRIBUTE_NUM = 100;
    private static int[] SubNum = new int[STOCKNUM];
    private static int matchNum = 0;

    private static Bucket[][][][] bucketlist = new Bucket[STOCKNUM][ATTRIBUTE_NUM][2][PART];
    private static BitSetVal[][] bitSet = new BitSetVal[STOCKNUM][MAX_SUB_NUM];
    private static ConNum[] conNum = new ConNum[STOCKNUM];
    private static Bucket[][] threadBucket = new Bucket[STOCKNUM][MAX_THREAD_Num];

    private static int SendThreadNum = 4;
    private static int match_thread_num = 1;
    private static int eventNum = 0;

    private static final int adjustSize = 20;
    private static final int baseMatchTime = 5;
    private static double AverMatchTime = 0;
    private static double LastMatchTime = 0;
    private static double LastMatchThread = 0;
    private static int SumEventNum = 0;
    private static double baseRate = 0.4;
    private static boolean adjustSwitch = false;

    private static final String mtFile = "resources/mt.txt";
    //dynamic version
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

        //initialize bucketlist
        for(int r = 0; r < STOCKNUM; r++) {
            SubNum[r] = 0;
            conNum[r] = new ConNum(ATTRIBUTE_NUM);
            for (int j = 0; j < ATTRIBUTE_NUM; j++) {
                for (int w = 0; w < 2; w++)
                    for (int i = 0; i < PART; i++)
                        bucketlist[r][j][w][i] = new Bucket();
            }
        }
        //initialise threadpool
        for(int i = 0;i < STOCKNUM;i++)
            for(int j= 0; j < MAX_THREAD_Num; j++){
                threadBucket[i][j] = new Bucket(ATTRIBUTE_NUM);
                for(int k=0;k<ATTRIBUTE_NUM;k++)
                    threadBucket[i][j].bitSet[k] = true;
            }


        ThreadPoolExecutor executorMatch = new ThreadPoolExecutor(0, Integer.MAX_VALUE,
                60L, TimeUnit.SECONDS,
                new SynchronousQueue<>());
        ThreadPoolExecutor executorSend = new ThreadPoolExecutor(8, 8,
                0L, TimeUnit.MILLISECONDS,
                new LinkedBlockingQueue<>());
        //set config
        Properties props = new Properties();
        props.put(StreamsConfig.APPLICATION_ID_CONFIG, "stream_index");
        props.put(StreamsConfig.BOOTSTRAP_SERVERS_CONFIG, KafkaServer);
        props.put(StreamsConfig.DEFAULT_KEY_SERDE_CLASS_CONFIG, Serdes.String().getClass());
        props.put(StreamsConfig.DEFAULT_VALUE_SERDE_CLASS_CONFIG, Serdes.String().getClass());

        Properties ProducerProps =  new Properties();
        ProducerProps.put("bootstrap.servers", KafkaServer);
        ProducerProps.put("key.serializer", "org.apache.kafka.common.serialization.StringSerializer");
        ProducerProps.put("value.serializer", ValueSerde.EventValSerde.class.getName());
        KafkaProducer<String, EventVal> producer = new KafkaProducer<>(ProducerProps);

        final StreamsBuilder index_builder = new StreamsBuilder();
        final StreamsBuilder match_builder = new StreamsBuilder();

        KStream<String, SubscribeVal> subscribe = index_builder.stream("Sub",
                Consumed.with(Serdes.String(), new ValueSerde.SubscribeSerde()));
        KStream<String, EventVal> event = match_builder.stream("Event",
                Consumed.with(Serdes.String(), new ValueSerde.EventSerde()));

        //parallel model
        class Parallel implements Runnable{
            private int threadIdx;
            private int type;
            private EventVal v;
            private CountDownLatch latch;
            private KafkaProducer<String, EventVal> Producer;
            private int curSubNum;

            private Parallel(int threadIdx, EventVal val, CountDownLatch latch){
                this.threadIdx = threadIdx;
                this.v = val;
                this.latch = latch;
                this.type = TPYE_MATCH;
            }
            private Parallel(int threadIdx, EventVal val, CountDownLatch latch, KafkaProducer<String, EventVal> producer, int n){
                this.threadIdx = threadIdx;
                this.v = val;
                this.latch = latch;
                this.Producer = producer;
                this.type = TYPE_SEND;
                this.curSubNum = n;
            }

            private void Match(){
                int stock_id = this.v.StockId;
                int attribute_num = v.AttributeNum;
                for (int i = 0; i < attribute_num; i++) {
                    if(!threadBucket[stock_id][threadIdx].bitSet[i])continue;
                    int attribute_id = this.v.eventVals[i].attributeId;
                    double val = this.v.eventVals[i].val;
                    int group = (int) (val / GROUP_WIDTH);
		    int size = bucketlist[stock_id][attribute_id][1][group].bucket.size();
		    for (int e=0; e < size;e++) {
			    if (bucketlist[stock_id][attribute_id][1][group].bucket.get(e).val < val) {
				    bitSet[stock_id][bucketlist[stock_id][attribute_id][1][group].bucket.get(e).Id].b = true;
			    }
		    }
		    for (int j = group - 1; j >= 0; j--) {
			    size = bucketlist[stock_id][attribute_id][1][j].bucket.size();
			    for (int e=0; e <size;e++) {
				    bitSet[stock_id][bucketlist[stock_id][attribute_id][1][j].bucket.get(e).Id].b = true;
			    }
		    }
		    size = bucketlist[stock_id][attribute_id][0][group].bucket.size();
		    for (int e=0;e<size;e++) {
			    if (bucketlist[stock_id][attribute_id][0][group].bucket.get(e).val > val) {
				    bitSet[stock_id][bucketlist[stock_id][attribute_id][0][group].bucket.get(e).Id].b = true;
			    }
		    }
		    for (int j = group + 1; j < PART; j++) {
			    size = bucketlist[stock_id][attribute_id][0][j].bucket.size();
			    for (int e=0;e<size;e++) {
				    bitSet[stock_id][bucketlist[stock_id][attribute_id][0][j].bucket.get(e).Id].b = true;
			    }
		    }
                }
            }
            private void Send(){
                int stock_id = this.v.StockId;
                int index = this.threadIdx;
                int stride = SendThreadNum;
                for (int i = index; i < this.curSubNum; i += stride) {
                    if (bitSet[stock_id][i].state) {
                        if (!bitSet[stock_id][i].b) {
                            matchNum++;
                            int id = i;
                            Thread thread = new Thread(() -> {
                                ProducerRecord<String, EventVal> record = new ProducerRecord<>(bitSet[stock_id][id].SubId, this.v);
                                try {
                                    Producer.send(record);
                                } catch (Exception x) {
                                    x.printStackTrace();
                                }
                            });
                            executorSend.execute(thread);
                        }else {
                            bitSet[stock_id][i].b = false;
                        }
                    }
                }
            }
            public void run(){
                switch (this.type) {
                    case TPYE_MATCH:
                        this.Match();
                        break;
                    case TYPE_SEND:
                        this.Send();
                        break;
                    default:
                        try{
                            throw new NullPointerException();
                        }catch (NullPointerException e) {
                            System.out.println("The type is error!");
                        }
                        break;
                }
                this.latch.countDown();
            }
        }
        //index structure insert
        subscribe.foreach((k,v)->{
            final  String subId = v.SubId;
            final int stock_id = v.StockId;
            final int sub_num_id = SubNum[stock_id];
            final int attributeNum = v.AttributeNum;
		
            //initialize bitset
            bitSet[stock_id][sub_num_id] = new BitSetVal(subId);
            //insert sub to bucketlist
            for(int i = 0; i < attributeNum; i++) {
                int attribute_id = v.subVals.get(i).attributeId;
                conNum[stock_id].ConSumNum++;
                conNum[stock_id].AttriConNum[attribute_id]++;
                double min_val = v.subVals.get(i).min_val;
                double max_val = v.subVals.get(i).max_val;
                int group = (int)(min_val / GROUP_WIDTH);
                bucketlist[stock_id][attribute_id][0][group].bucket.add(new List(sub_num_id, min_val));
                group = (int)(max_val / GROUP_WIDTH);
                bucketlist[stock_id][attribute_id][1][group].bucket.add(new List(sub_num_id, max_val));
            }
            SubNum[stock_id]++;
        });
        //filestream
        File file = new File(mtFile);
        FileWriter fw = null;
        try {
            fw = new FileWriter(file, true);
        } catch (IOException e) {
            System.err.println("write file failed");
            e.printStackTrace();
        }
        BufferedWriter bw = new BufferedWriter(fw);

        //matcher
        event.foreach((k,v) -> {
            int stock_id = v.StockId;

            //compute event access delay
            long tmpTime = System.currentTimeMillis();
            //EventVal eVal = value;
            v.EventArriveTime = tmpTime - v.EventProduceTime;
            //preprocess
            final  CountDownLatch latch = new CountDownLatch(match_thread_num);
            //System.out.println("Stock Id: " + stock_id + " Attribute Num: " + attributeNum);
            //match
            int curSubNum = SubNum[stock_id];
            tmpTime = System.nanoTime();
            for (int i = 0; i < match_thread_num; i++) {
                Parallel s = new Parallel(i, v, latch);
                executorMatch.execute(s);
            }
            try {
                latch.await();
            }catch (Exception e){
                e.printStackTrace();
            }

            double per_match_time = (System.nanoTime() - tmpTime)/1000000.0;
            String s = curSubNum + " " + per_match_time + " " + match_thread_num;

            //sender
            final  CountDownLatch Slatch = new CountDownLatch(SendThreadNum);
            v.EventStartSendTime = System.currentTimeMillis();
            for (int i = 0; i < SendThreadNum; i++) {
                Parallel p = new Parallel(i, v, Slatch, producer, curSubNum);
                executorMatch.execute(p);
            }

            System.out.println("event:" + SumEventNum + " " + s + " matchNum:" + matchNum);

            //adjust
            if(eventNum == adjustSize) {
                long tmp = System.nanoTime();
                int thread_tmp = match_thread_num;
                AverMatchTime = AverMatchTime / adjustSize;
                s = s + " " + AverMatchTime;

                if(!adjustSwitch && AverMatchTime > baseMatchTime * baseRate){
                    LastMatchThread = match_thread_num;
                    match_thread_num = 2;
                    LastMatchTime = AverMatchTime;
                    adjustSwitch = true;
                }else if(AverMatchTime > baseMatchTime * baseRate) {
                        if(match_thread_num == 1){
                            LastMatchThread = match_thread_num;
                            match_thread_num = 2;
                            LastMatchTime = AverMatchTime;
                        }else{
                            double e = (AverMatchTime - LastMatchTime)/LastMatchTime * match_thread_num/LastMatchThread;
                            if(match_thread_num < LastMatchThread)e=-e;
                            int o = (int)(match_thread_num * (1 - e));
                            o = o > MAX_THREAD_Num ? MAX_THREAD_Num : o;
                            if (o > 0 && o != match_thread_num) {
                                LastMatchThread = match_thread_num;
                                LastMatchTime = AverMatchTime;
                                match_thread_num = o;
                            }
                        }
                }else if(baseMatchTime * baseRate - 1> AverMatchTime){
                    if(match_thread_num > 1){
                        LastMatchThread = match_thread_num;
                        LastMatchTime = AverMatchTime;
                        match_thread_num--;
                    }
                }
                if(match_thread_num != thread_tmp){
                    for(int i = 0;i < match_thread_num; i++){
                        threadBucket[stock_id][i].executeNum = 0;
                        for(int j = 0;j<ATTRIBUTE_NUM;j++){
                            threadBucket[stock_id][i].bitSet[j] = false;
                        }
                    }
                    boolean[] bit = new boolean[ATTRIBUTE_NUM];
                    for(int i=0;i<ATTRIBUTE_NUM;i++)bit[i]=false;
                    for(int i=0;i<ATTRIBUTE_NUM;i++){
                        int max = 0;
                        int n = 0;
                        for(int j=0;j<ATTRIBUTE_NUM;j++){
                            if(bit[j])continue;
                            if(max < conNum[stock_id].AttriConNum[j]){
                                max = conNum[stock_id].AttriConNum[j];
                                n = j;
                            }
                        }
                        bit[n] = true;
                        int t = 0;
                        int min = Integer.MAX_VALUE;
                        for(int j=0;j<match_thread_num;j++){
                            if(min > threadBucket[stock_id][j].executeNum){
                                min = threadBucket[stock_id][j].executeNum;
                                t = j;
                            }
                        }
                        threadBucket[stock_id][t].executeNum+=max;
                        threadBucket[stock_id][t].bitSet[n] = true;
                    }
                }
                s = s + " " + (System.nanoTime() - tmp)/1000.0;
                System.out.println("Aver_Time: " + AverMatchTime + "Lthread:" + LastMatchThread + "thread:" + match_thread_num + "thread adjust time: " + (System.nanoTime() - tmp) + "\n");
                AverMatchTime = 0;
                eventNum = 0;
            }
            eventNum++;
            SumEventNum++;
            AverMatchTime += per_match_time;
            matchNum = 0;
            try {
                bw.write(s + "\n");
            } catch (IOException e) {
                e.printStackTrace();
            }
            try {
                Slatch.await();
            }catch (Exception e){
                e.printStackTrace();
            }
        });

        final Topology index_topology = index_builder.build();
        final KafkaStreams stream_index = new KafkaStreams(index_topology, props);
        props.put(StreamsConfig.APPLICATION_ID_CONFIG, "stream_matcher");
        final Topology matcher_topology = match_builder.build();
        final KafkaStreams stream_matcher = new KafkaStreams(matcher_topology, props);
        final CountDownLatch latch = new CountDownLatch(1);
        // attach shutdown handler to catch control-c
        Runtime.getRuntime().addShutdownHook(new Thread("streams-shutdown-hook") {
            @Override
            public void run() {
                stream_index.close();
                stream_matcher.close();
                producer.close();
                executorMatch.shutdown();
                executorSend.shutdown();
                try {
                    bw.flush();
                    bw.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
                latch.countDown();
            }
        });
        try {
            stream_index.start();
            stream_matcher.start();
            latch.await();
        } catch (Throwable e) {
            System.exit(1);
        }
        System.exit(0);
    }

}
