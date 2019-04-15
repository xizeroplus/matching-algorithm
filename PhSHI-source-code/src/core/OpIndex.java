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

public class OpIndex {

    private final static int STOCKNUM = 2;
    private final static int ATTRIBUTE_NUM = 100;
    private final static int OPNUM = 2;
    private final static int MAX_SUB_NUM = 1000001;
    private final static int MAX_THREAD_Num = 33;
    private static int match_thread_num = 1;

    private static Index_Op[][] indexLists = new Index_Op[STOCKNUM][ATTRIBUTE_NUM];
    private static SubSet[][][] sets = new SubSet[STOCKNUM][ATTRIBUTE_NUM][MAX_SUB_NUM];
    private static int matchNum = 0;
    private static int eventNum = 0;

    private static ConNum[] conNum = new ConNum[STOCKNUM];
    private static Bucket[][] threadBucket = new Bucket[STOCKNUM][MAX_THREAD_Num];

    //main
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

        ThreadPoolExecutor executorMatch = new ThreadPoolExecutor(0, Integer.MAX_VALUE,
                60L, TimeUnit.SECONDS,
                new SynchronousQueue<>());
        ThreadPoolExecutor executorSend = new ThreadPoolExecutor(8, 8,
                0L, TimeUnit.MILLISECONDS,
                new LinkedBlockingQueue<>());
        //parallel model
        class Parallel implements Runnable{
            private int threadIdx;
            private EventVal v;
            private KafkaProducer<String, EventVal> Producer;
            private CountDownLatch latch;
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
                    if(indexLists[stock_id][attribute_id].Pivot){
                        for(int r = 0; r < attribute_num; r++){
                            double val = this.v.eventVals[r].val;
                            if(indexLists[stock_id][attribute_id].opList.opBuckets[0].bits[r]) {
                                for (List e : indexLists[stock_id][attribute_id].opList.opBuckets[0].opBucketLists[r].opBucketList) {
                                    if (e.val <= val) {
                                        try {
                                            sets[stock_id][attribute_id][e.Id].sem.acquire(2);
                                            sets[stock_id][attribute_id][e.Id].un_matched_num--;
                                        } catch (InterruptedException w) {
                                            w.printStackTrace();
                                        } finally {
                                            sets[stock_id][attribute_id][e.Id].sem.release(2);
                                        }
                                        if (sets[stock_id][attribute_id][e.Id].un_matched_num == 0) {
                                            int y = e.Id;
                                            matchNum++;
                                            Thread thread = new Thread(() -> {
                                                ProducerRecord<String, EventVal> record = new ProducerRecord<>(sets[stock_id][attribute_id][y].SubId, this.v);
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
                            if(indexLists[stock_id][attribute_id].opList.opBuckets[1].bits[r]) {
                                for (List e : indexLists[stock_id][attribute_id].opList.opBuckets[1].opBucketLists[r].opBucketList) {
                                    if (e.val >= val) {
                                        try {
                                            sets[stock_id][attribute_id][e.Id].sem.acquire(2);
                                            sets[stock_id][attribute_id][e.Id].un_matched_num--;
                                        } catch (InterruptedException w) {
                                            w.printStackTrace();
                                        } finally {
                                            sets[stock_id][attribute_id][e.Id].sem.release(2);
                                        }
                                        if (sets[stock_id][attribute_id][e.Id].un_matched_num == 0) {
                                            int y = e.Id;
                                            matchNum++;
                                            Thread thread = new Thread(() -> {
                                                ProducerRecord<String, EventVal> record = new ProducerRecord<>(sets[stock_id][attribute_id][y].SubId, this.v);
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
                    }
                }
            }
            public void run(){
                this.Match();
                this.latch.countDown();
            }
        }
        //initialize indexLists
        for(int j = 0; j < STOCKNUM; j++){
            conNum[j] = new ConNum(ATTRIBUTE_NUM);
            for(int i = 0; i < ATTRIBUTE_NUM; i++){
                indexLists[j][i] = new Index_Op(OPNUM, ATTRIBUTE_NUM);
            }
            for(int i= 0; i < MAX_THREAD_Num; i++){
                threadBucket[j][i] = new Bucket(ATTRIBUTE_NUM);
            }
        }
        //set stream config
        Properties props = new Properties();
        props.put(StreamsConfig.APPLICATION_ID_CONFIG, "stream_index");
        props.put(StreamsConfig.BOOTSTRAP_SERVERS_CONFIG, KafkaServer);
        props.put(StreamsConfig.DEFAULT_KEY_SERDE_CLASS_CONFIG, Serdes.String().getClass());
        props.put(StreamsConfig.DEFAULT_VALUE_SERDE_CLASS_CONFIG, Serdes.String().getClass());
        //set producer config
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

        //index structure insert
        subscribe.foreach((k,v)->{
            final  String subId = v.SubId;
            final int stock_id = v.StockId;
            final int pivotId = v.Pivot_Attri_Id;
            final int sub_num_id = indexLists[stock_id][pivotId].SubNum++;
            final int attributeNum = v.AttributeNum;

            conNum[stock_id].ConSumNum++;
            conNum[stock_id].AttriConNum[pivotId]++;

            System.out.println("Client Name: " + subId + " Client Num Id: " + sub_num_id +
                    " Sub Stock Id: " + stock_id + " Attribute Num: " + attributeNum);
            //insert set
            sets[stock_id][pivotId][sub_num_id] = new SubSet(attributeNum, subId);
            //insert sub
            indexLists[stock_id][pivotId].Pivot = true;
            for(int i = 0; i < attributeNum; i++) {
                indexLists[stock_id][pivotId].opList.opBuckets[0].bits[v.subVals.get(i).attributeId] = true;
                indexLists[stock_id][pivotId].opList.opBuckets[1].bits[v.subVals.get(i).attributeId] = true;
                indexLists[stock_id][pivotId].opList.opBuckets[0].opBucketLists[v.subVals.get(i).attributeId].
                        opBucketList.add(new List(sub_num_id, v.subVals.get(i).min_val));
                indexLists[stock_id][pivotId].opList.opBuckets[1].opBucketLists[v.subVals.get(i).attributeId].
                        opBucketList.add(new List(sub_num_id, v.subVals.get(i).max_val));
            }
        });
        File file = new File("resources/mt-op.txt");
        FileWriter fw = null;
        try {
            fw = new FileWriter(file, true);
        } catch (IOException e) {
            System.err.println("write file failed!");
            e.printStackTrace();
        }
        assert fw != null;
        BufferedWriter bw = new BufferedWriter(fw);

        int[] matchT = {1,2,4,8,16,32};
        //matcher
        event.foreach((k, v) -> {
            int stock_id = v.StockId;
            if(eventNum%2000==0){
                match_thread_num = matchT[(eventNum/2000)%6];

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
                    int n = 0;//每次选中约束数目最大的属性id
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
            final CountDownLatch T_latch = new CountDownLatch(match_thread_num);
            //System.out.println("Stock Id: " + stock_id + " Attribute Num: " + attributeNum);
            //launch match
            tmpTime = System.nanoTime();
            for (int i = 0; i < match_thread_num; i++) {
                Parallel s = new Parallel(i, v, T_latch, producer);
                executorMatch.execute(s);
            }
            try {
                T_latch.await();
            }catch (Exception e){
                e.printStackTrace();
            }
            int stockId = v.StockId;
            for(int i = 0; i < ATTRIBUTE_NUM; i++){
                for(int j=0;j<indexLists[stockId][i].SubNum;j++){
                    sets[stockId][i][j].un_matched_num = sets[stockId][i][j].attribute_num << 1;
                }
            }
            String s = String.valueOf((System.nanoTime() - tmpTime)/1000000.0);
            eventNum++;
            System.out.println(eventNum + " " + s+ " " +matchNum + " " + match_thread_num);
            matchNum = 0;
            try {
                bw.write(s + "\n");
            } catch (IOException e) {
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
