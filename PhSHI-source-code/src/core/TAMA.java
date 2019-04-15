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

public class TAMA {
    private final static int LEVEL = 11;
    private final static int MAX_VALUE = 1000;
    private final static int MAX_SUB_NUM = 1000001;
    private final static int MAX_THREAD_Num = 33;
    private final static double[] CELL_WIDTH = new double[LEVEL];
    private final static int STOCKNUM = 2;
    private final static int ATTRIBUTE_NUM = 100;
    private static int[] SubNum = new int[STOCKNUM];

    private static Bucket[][][][] bucketlist = new Bucket[STOCKNUM][ATTRIBUTE_NUM][LEVEL][];
    private static SubSet[][] sets = new SubSet[STOCKNUM][MAX_SUB_NUM];

    private static int match_thread_num = 1;
    private static int matchNum = 0;
    private static int eventNum = 0;

    private static ConNum[] conNum = new ConNum[STOCKNUM];
    private static Bucket[][] threadBucket = new Bucket[STOCKNUM][MAX_THREAD_Num];

    private static final String mtFile = "resources/mt-tama.txt";

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

        //initialize bucketlis
        int[] tmp_t = new int[LEVEL];
        for(int i = 0; i < LEVEL; i++){
            tmp_t[i] = (int) Math.pow(2, i);
        }
        for(int r = 0; r < STOCKNUM; r++){
            conNum[r] = new ConNum(ATTRIBUTE_NUM);
            for(int j = 0; j < ATTRIBUTE_NUM; j++){
                for(int w = 0; w < LEVEL; w++) {
                    CELL_WIDTH[w] = (double) MAX_VALUE / (double) tmp_t[w];
                    bucketlist[r][j][w] = new Bucket[tmp_t[w]];
                    for(int i = 0;i < tmp_t[w];i++)
                        bucketlist[r][j][w][i] = new Bucket();
                }
            }
        }
        //initialise threadpool
        for(int i = 0;i < STOCKNUM;i++) {
            SubNum[i] = 0;
            for (int j = 0; j < MAX_THREAD_Num; j++) {
                threadBucket[i][j] = new Bucket(ATTRIBUTE_NUM);
            }
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

        System.out.println("Stream Id: streams-match Max Stock Num: " + STOCKNUM + " Max Attribute Num: " + ATTRIBUTE_NUM +
                "\nPart Num: " + LEVEL + " Max Value: " + MAX_VALUE );

        //parallel model
        class Parallel implements Runnable{
            private int threadIdx;
            private EventVal v;
            private CountDownLatch latch;
            private KafkaProducer<String, EventVal> Producer;

            private Parallel(int threadIdx, EventVal val, CountDownLatch latch, KafkaProducer<String, EventVal> producer){
                this.threadIdx = threadIdx;
                this.v = val;
                this.latch = latch;
                this.Producer = producer;
            }

            private void Match(){
                int stock_id = this.v.StockId;
                int attribute_num = v.AttributeNum;
                for (int i = 0; i < attribute_num; i++) {
                    if(!threadBucket[stock_id][threadIdx].bitSet[i])continue;
                    int attribute_id = this.v.eventVals[i].attributeId;
                    double val = this.v.eventVals[i].val;
                    for(int j = 0;j < LEVEL;j++){
                        int t = (int)(val / CELL_WIDTH[j]);
                        for(List e:bucketlist[stock_id][attribute_id][j][t].bucket){
                            try{
                                sets[stock_id][e.Id].sem.acquire(2);
                                sets[stock_id][e.Id].matched_num++;
                            }catch(InterruptedException r){
                                r.printStackTrace();
                            }finally {
                                sets[stock_id][e.Id].sem.release(2);
                            }
                            if (sets[stock_id][e.Id].matched_num == sets[stock_id][e.Id].attribute_num) {
                                int id = e.Id;
                                matchNum ++;
                                Thread thread = new Thread(() -> {
                                    ProducerRecord<String, EventVal> record = new ProducerRecord<>(sets[stock_id][id].SubId, this.v);
                                    try {
                                        Producer.send(record);
                                    } catch (Exception x) {
                                        x.printStackTrace();
                                    }
                                });
                                executorSend.execute(thread);
                            }
                        }
                    }

                }
            }
            public void run(){
                this.Match();
                this.latch.countDown();
            }
        }

        //index structure insert
        subscribe.foreach((k,v)->{
            final  String subId = v.SubId;
            final int stock_id = v.StockId;
            final  int sub_num_id = SubNum[stock_id];
            final int attributeNum = v.AttributeNum;
            sets[stock_id][sub_num_id] = new SubSet(attributeNum,subId);
            System.out.println("Client Name: " + subId + " Client Num Id: " + sub_num_id +
                    " Sub Stock Id: " + stock_id + " Attribute Num: " + attributeNum);
            for(int i = 0;i < attributeNum;i++){
                int attribute_id = v.subVals.get(i).attributeId;
                conNum[stock_id].ConSumNum++;
                conNum[stock_id].AttriConNum[attribute_id]++;
                double low = v.subVals.get(i).min_val;
                double high = v.subVals.get(i).max_val;
                int start = 0;
                int lmark = 0;
                int rmark = 0;
                for(int j = 0;j < LEVEL ;j++){
                    int left = (int)(low / CELL_WIDTH[j]);
                    int right = (int)(high / CELL_WIDTH[j]);
                    start = j;
                    if(right - left > 1){
                        if(right - left == 2){
                            bucketlist[stock_id][attribute_id][j][left + 1].bucket.add(new List(sub_num_id));
                            lmark = left + 1;
                            rmark = lmark;
                        }else {
                            bucketlist[stock_id][attribute_id][j][left + 1].bucket.add(new List(sub_num_id));
                            bucketlist[stock_id][attribute_id][j][right - 1].bucket.add(new List(sub_num_id));
                            lmark = left + 1;
                            rmark = right - 1;
                        }
                        break;
                    }
                }
                for(int j = start + 1;j < LEVEL - 1;j++){
                    int left = (int)(low / CELL_WIDTH[j]);
                    int right = (int)(high / CELL_WIDTH[j]);
                    if(((left + 1 ) / 2) != lmark)
                        bucketlist[stock_id][attribute_id][j][left + 1].bucket.add(new List(sub_num_id));
                    lmark = left + 1;
                    if(((right - 1) / 2) != rmark)
                        bucketlist[stock_id][attribute_id][j][right - 1].bucket.add(new List(sub_num_id));
                    rmark = right - 1;
                }
                int left = (int)(low / CELL_WIDTH[LEVEL - 1]);
                int right = (int)(high / CELL_WIDTH[LEVEL - 1]);
                bucketlist[stock_id][attribute_id][LEVEL - 1][left].bucket.add(new List(sub_num_id));
                if(left != right)
                    bucketlist[stock_id][attribute_id][LEVEL - 1][right].bucket.add(new List(sub_num_id));
            }
            SubNum[stock_id]++;
        });
        File file = new File(mtFile);
        FileWriter fw = null;
        try {
            fw = new FileWriter(file, true);
        } catch (IOException e) {
            e.printStackTrace();
        }
        BufferedWriter bw = new BufferedWriter(fw);

        int[] matchT = {1,2,4,8,16,32};
        //matcher
        event.foreach((k, v) -> {
            int stock_id = v.StockId;
            if(eventNum%2000==0){
                match_thread_num = matchT[(eventNum/2000)%6];
		        //match_thread_num = 32;
                //Task division
                //Empty threadBucket
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
            //compute event access delay
            long tmpTime = System.currentTimeMillis();
            v.EventArriveTime = tmpTime - v.EventProduceTime;
            final  CountDownLatch latch = new CountDownLatch(match_thread_num);
            //match
            tmpTime = System.nanoTime();
            for (int i = 0; i < match_thread_num; i++) {
                Parallel s = new Parallel(i, v, latch, producer);
                executorMatch.execute(s);
            }
            try {
                latch.await();
            }catch (Exception e){
                e.printStackTrace();
            }
            for(int i=0;i<SubNum[stock_id];i++){
                sets[stock_id][i].matched_num = 0;
            }
            String s = String.valueOf((System.nanoTime() - tmpTime)/1000000.0);
            eventNum++;
            System.out.println(eventNum+ " " + s + " " + matchNum+" " + match_thread_num);
            matchNum = 0;
            try {
                bw.write(s + "\n");
            } catch (IOException e) {
                System.err.println("write file failed!");
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
