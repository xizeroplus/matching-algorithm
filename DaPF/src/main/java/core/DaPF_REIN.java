package core;

import org.apache.kafka.common.metrics.stats.Count;
import org.apache.log4j.*;

import MySerdes.ValueSerde;
import Structure.*;
import org.apache.kafka.clients.producer.KafkaProducer;
import org.apache.kafka.clients.producer.ProducerRecord;
import org.apache.kafka.common.serialization.Serdes;
import org.apache.kafka.streams.*;
import org.apache.kafka.streams.kstream.KStream;
//import utils.InfluxdbUtil;
import utils.MatchSendStruct;
import utils.SendPair;

import javax.swing.undo.CannotUndoException;
import java.io.*;
import java.util.ArrayList;
import java.util.Properties;
import java.util.concurrent.*;

public class DaPF_REIN {
    // matcher capabilities
    private static int ATTRIBUTE_NUM = 60;
    private final static int MAX_VALUE = 1000;
    private final static int MAX_SUB_NUM = 1000101;
    private final static int PART = 1000;
    private final static double GROUP_WIDTH = (double)MAX_VALUE / (double)PART;
    private final static int STOCK_NUM = 2;
    private final static int TYPE_MATCH = 0;
    private final static int TYPE_SEND = 1;

    private final static int MAX_MATCH_THREAD_NUM = 40;
    private final static int MAX_SEND_THREAD_NUM = 20;
    private static int MIN_SEND_THREAD_NUM = 4;

    // dynamical control vars
    private static double BASE_RATE = 0.3;
    private static double SEND_BASE_RATE = 0.1;
    private static double RATE_OF_EVENT_VEL = 0.2;
    private static final int MatchWinSize = 20;
    private static final int MatchWinAvgSize = 20;
    private static final int SendTimeAvgStatisticSize = 1000;

    private static int ExpTotalDelay = 50;
    private static int GlobalExpMatchTimeSet = 15;
    private static int ExpMatchTime = GlobalExpMatchTimeSet;
    private static int ExpSendTime = ExpTotalDelay - ExpMatchTime - 10;

    private static int SummaryThreadNum = 10;
    private static int CurrentMatchThreadNum = 1;
    private static int CurrentSendThreadNum = 4;
    private static int LastSendThredNum = CurrentSendThreadNum;

    private static boolean adjustSwitch = false;
    private static double AvgMatchTime = 0;
    private static double SumDispatchTime = 0;
    private static double AvgDispatchTime = 0;
    private static double AvgSendTime = 0;

    private static double LastMatchTime = 0;
    private static double LastSendTime = 0;
    private static int perThreadExpQueueLength = 1000;
    private static double addOrMinusPercent = 0.1;
    private static int LastMatchThread = 0;

    /* shared variables between threads */
    private static int[] SubNum = new int[STOCK_NUM];
    private static Bucket[][][][] BucketList;
    private static BitSetVal[][] SubBitSet = new BitSetVal[STOCK_NUM][MAX_SUB_NUM];
    private static ConNum[] conNum = new ConNum[STOCK_NUM];
    private static Bucket[][] ThreadBucket = new Bucket[STOCK_NUM][MAX_MATCH_THREAD_NUM];

    /* define the send buffer */
    private static MatchSendStruct MatchSummarySetBuffer;

    /* statistic vars */
    private static int SumEventNum = 0;
    private static int EventNum = 0;


    /* init the kafka delay time statistic */
    private static long AvgKafkaDelayTime;
    private static float MinAvgKafkaDelayTime = Float.MAX_VALUE;
    private static long WindowBeginTime = 0;
    private static long WindowEndTime = 0;

    private static double SendFeedBackTime[][] = new double[MatchWinSize][MAX_SEND_THREAD_NUM];
    private double perEventSumTime;
    /* influxdb connection **/
    // private static InfluxdbUtil influx;

    //dynamic version rein
    public static void main(String[] args) {
        // parse arguments from command line
        String config_filename = "";

        if(args.length < 3){
            System.err.println("Usage: DaPF_REIN -matchconfigfile -expdelaytime -maxattr ");
            System.exit(1);
        }
        try{
            config_filename = args[0];

            ExpTotalDelay = Integer.parseInt(args[1]);
            ExpSendTime = ExpTotalDelay - GlobalExpMatchTimeSet - 10;

            ATTRIBUTE_NUM = Integer.parseInt(args[2]);
            BucketList = new Bucket[STOCK_NUM][ATTRIBUTE_NUM][2][PART];
        } catch (Throwable e){
            System.err.println("Usage: DaPF_REIN -matchconfigfile -expdelaytime -maxattr ");
            e.printStackTrace();
            System.exit(1);
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

        // logger
        Logger logger = Logger.getLogger(DaPF_REIN.class);
        SimpleLayout layout = new SimpleLayout();
        try{
            FileAppender logFileAppender=new FileAppender(layout, properties.getProperty("LogFile"));
            logger.addAppender(logFileAppender);
        } catch (Throwable e) {
            System.err.println("Open log file error!");
        }
        logger.setLevel(Level.INFO);

        // set kafka connection config
        String KafkaServer = properties.getProperty("KafkaServer");
        String securityProto = properties.getProperty("security.protocol");
        String saslName = properties.getProperty("sasl.kerberos.service.name");
//        String influx_filename = properties.getProperty("InfluxdbConfigFile");

        Properties props = new Properties();
        props.put(StreamsConfig.APPLICATION_ID_CONFIG, "stream_index");
        props.put(StreamsConfig.BOOTSTRAP_SERVERS_CONFIG, KafkaServer);
        props.put(StreamsConfig.DEFAULT_KEY_SERDE_CLASS_CONFIG, Serdes.String().getClass());
        props.put(StreamsConfig.DEFAULT_VALUE_SERDE_CLASS_CONFIG, Serdes.String().getClass());

        Properties ProducerProps =  new Properties();
        ProducerProps.put("bootstrap.servers", KafkaServer);
        ProducerProps.put("key.serializer", "org.apache.kafka.common.serialization.StringSerializer");
        ProducerProps.put("value.serializer", ValueSerde.EventValSerde.class.getName());

        // security configuration set
        if(securityProto != null && saslName != null){
            props.put("security.protocol", securityProto);
            props.put("sasl.kerberos.service.name", saslName);
        } else {
            logger.warn("WARNING: no authentication configuration");
        }

        final StreamsBuilder index_builder = new StreamsBuilder();
        final StreamsBuilder match_builder = new StreamsBuilder();

        KStream<String, SubscribeVal> subscribe = index_builder.stream("Sub",
                Consumed.with(Serdes.String(), new ValueSerde.SubscribeSerde()));
        KStream<String, EventVal> event = match_builder.stream("Event",
                Consumed.with(Serdes.String(), new ValueSerde.EventSerde()));

        // initialize BucketList
        for(int r = 0; r < STOCK_NUM; r++) {
            SubNum[r] = 0;
            conNum[r] = new ConNum(ATTRIBUTE_NUM);
            for (int j = 0; j < ATTRIBUTE_NUM; j++) {
                for (int w = 0; w < 2; w++)
                    for (int i = 0; i < PART; i++)
                        BucketList[r][j][w][i] = new Bucket();
            }
        }
        // initialize bitmap
        for(int i = 0;i < STOCK_NUM;i++)
            for(int j= 0; j < MAX_MATCH_THREAD_NUM; j++){
                ThreadBucket[i][j] = new Bucket(ATTRIBUTE_NUM);
                for(int k=0;k<ATTRIBUTE_NUM;k++)
                    ThreadBucket[i][j].bitSet[k] = true;
            }

        // initialize match thread pool
        ThreadPoolExecutor executorMatch = new ThreadPoolExecutor(0, Integer.MAX_VALUE,
                60L, TimeUnit.SECONDS,
                new SynchronousQueue<>());
        ThreadPoolExecutor executorSend = new ThreadPoolExecutor(0, Integer.MAX_VALUE,
                60L, TimeUnit.SECONDS,
                new SynchronousQueue<>());
        // initialize summary thread pool
        ThreadPoolExecutor executorSummary = new ThreadPoolExecutor(8, MAX_MATCH_THREAD_NUM,
                10000L, TimeUnit.MILLISECONDS,
                new LinkedBlockingQueue<>());

        // construct the influx connection object
        // influx = InfluxdbUtil.setUp(influx_filename,"matchTime", "matcherState");

        /* define producer API send thread */
        class MatchResultSendThread extends Thread{
            private volatile LinkedBlockingQueue<SendPair> localSendQueue;
            private KafkaProducer<String, EventVal> localProducer;
            private int threadIdx;
            private int eIdx;

            private MatchResultSendThread(KafkaProducer<String, EventVal> prod, int Idx,
                                          LinkedBlockingQueue<SendPair> queue, int eventIdx){
                localSendQueue = queue;
                localProducer = prod;
                threadIdx = Idx;
                eIdx = eventIdx;
            }

            private void addLocalQueue(SendPair pair){
                try{
                    localSendQueue.put(pair);
                } catch (Exception e){
                    logger.error(String.format("+ From thread %d: add local queue error!",
                            threadIdx));
                    e.printStackTrace();
                }
            }

            @Override
            public void run(){

                int localCount = 0;
                double localSum = 0;

                SendPair localSendPair;

                // work
                try{
                    while((localSendPair = localSendQueue.poll()) != null ){
                        ProducerRecord<String, EventVal> record;

                        record = new ProducerRecord<>(localSendPair.subName, localSendPair.eval);

                        // send data
                        try {
                            localProducer.send(record);
                        } catch (Exception e) {
                            logger.error(String.format("+ From thread %d: kafka send error!",threadIdx));
                            e.printStackTrace();
                        }

                        // statistic average event detain time
                        localSum += (System.currentTimeMillis()-localSendPair.eval.EventArriveTime);
                        localCount++;
                    }

                    SendFeedBackTime[eIdx % MatchWinSize][threadIdx] = localSum / localCount;
                    logger.info(String.format("+ From send %d: event %d detained %f ms",
                            threadIdx, eIdx, localSum / localCount));
                } catch (Exception e) {
                    e.printStackTrace();
                    logger.error(String.format("+ From thread %d: poll local queue interrupt error!",
                            threadIdx));
                }
            }

        }
        // create send thread instances
        ArrayList<KafkaProducer<String, EventVal>> allProducers = new ArrayList<>();
        for(int i = 0; i < MAX_SEND_THREAD_NUM; i++){
            allProducers.add(new KafkaProducer<>(ProducerProps));
        }

        /* parallel match and summary client model */
        class Parallel implements Runnable{
            private int threadIdx;
            private int type;
            private EventVal v;
            private CountDownLatch latch;
            private int curSubNum;

            private Parallel(int threadIdx, EventVal val, CountDownLatch latch){
                this.threadIdx = threadIdx;
                this.v = val;
                this.latch = latch;
                this.type = TYPE_MATCH;
            }
            private Parallel(int threadIdx, EventVal val, CountDownLatch latch, int n){
                this.threadIdx = threadIdx;
                this.v = val;
                this.latch = latch;
                this.type = TYPE_SEND;
                this.curSubNum = n;
            }

            private void Match(){
                int stock_id = this.v.StockId;
                int attribute_num = v.AttributeNum;

                for (int i = 0; i < attribute_num; i++) {
                    if(!ThreadBucket[stock_id][threadIdx].bitSet[i]) continue;

                    int attribute_id = this.v.eventVals[i].attributeId;
                    double val = this.v.eventVals[i].val;
                    int group = (int) (val / GROUP_WIDTH);
                    int size = BucketList[stock_id][attribute_id][1][group].bucket.size();
                    for (int e=0; e < size;e++) {
                        if (BucketList[stock_id][attribute_id][1][group].bucket.get(e).val < val) {
                            SubBitSet[stock_id][BucketList[stock_id][attribute_id][1][group].bucket.get(e).Id].b = true;
                        }
                    }
                    for (int j = group - 1; j >= 0; j--) {
                        size = BucketList[stock_id][attribute_id][1][j].bucket.size();
                        for (int e=0; e <size;e++) {
                            SubBitSet[stock_id][BucketList[stock_id][attribute_id][1][j].bucket.get(e).Id].b = true;
                        }
                    }
                    size = BucketList[stock_id][attribute_id][0][group].bucket.size();
                    for (int e=0;e<size;e++) {
                        if (BucketList[stock_id][attribute_id][0][group].bucket.get(e).val > val) {
                            SubBitSet[stock_id][BucketList[stock_id][attribute_id][0][group].bucket.get(e).Id].b = true;
                        }
                    }
                    for (int j = group + 1; j < PART; j++) {
                        size = BucketList[stock_id][attribute_id][0][j].bucket.size();
                        for (int e=0;e<size;e++) {
                            SubBitSet[stock_id][BucketList[stock_id][attribute_id][0][j].bucket.get(e).Id].b = true;
                        }
                    }
                }
            }

            private void summary(){
                int stock_id = this.v.StockId;
                int index = this.threadIdx;
                int stride = SummaryThreadNum;
                for (int i = index; i < this.curSubNum; i += stride) {
                    if (SubBitSet[stock_id][i].state) {
                        if (!SubBitSet[stock_id][i].b) {
                            /* push to the send set*/
                            MatchSummarySetBuffer.subSet.add(SubBitSet[stock_id][i].SubId);
                        } else {
                            SubBitSet[stock_id][i].b = false;
                        }
                    }
                }
            }

            public void run(){
                switch (this.type) {
                    case TYPE_MATCH:
                        this.Match();
                        break;
                    case TYPE_SEND:
                        this.summary();
                        break;
                    default:
                        try{
                            throw new NullPointerException();
                        }catch (NullPointerException e) {
                            logger.error("The type is error!");
                        }
                        break;
                }
                this.latch.countDown();
            }
        }

        /* subscribe handler */
        subscribe.foreach((k,v)->{
            final String subId = v.SubId;
            final int stock_id = v.StockId;
            final int sub_num_id = SubNum[stock_id];
            final int attributeNum = v.AttributeNum;

            //initialize bitset
            SubBitSet[stock_id][sub_num_id] = new BitSetVal(subId);
            //insert sub to BucketList
            for(int i = 0; i < attributeNum; i++) {
                int attribute_id = v.subVals.get(i).attributeId;
                conNum[stock_id].ConSumNum++;
                conNum[stock_id].AttriConNum[attribute_id]++;
                double min_val = v.subVals.get(i).min_val;
                double max_val = v.subVals.get(i).max_val;
                int group = (int)(min_val / GROUP_WIDTH);
                BucketList[stock_id][attribute_id][0][group].bucket.add(new List(sub_num_id, min_val));
                group = (int)(max_val / GROUP_WIDTH);
                BucketList[stock_id][attribute_id][1][group].bucket.add(new List(sub_num_id, max_val));
            }

            if(SubNum[stock_id] % 10000 == 0){
                logger.info(String.format("Client Name: %s, Num Id: %d, Attribute Num: %d",
                        subId,sub_num_id,attributeNum));
            }

            SubNum[stock_id]++;
        });

        /* matcher(event handler) */
        event.foreach((k,v) -> {
            int stock_id = v.StockId;

            // compute event access delay
            long eventArriveTime = System.currentTimeMillis();
            v.EventArriveTime = eventArriveTime;

            // pre-process
            final CountDownLatch matchLatch = new CountDownLatch(CurrentMatchThreadNum);

            // match
            int curSubNum = SubNum[stock_id];
            long tmpTime = System.nanoTime();
            for (int i = 0; i < CurrentMatchThreadNum; i++) {
                Parallel s = new Parallel(i, v, matchLatch);
                executorMatch.execute(s);
            }
            try {
                matchLatch.await();
            } catch (Exception e){
                logger.error("Match countdown latch wait error!");
                e.printStackTrace();
            }

            double per_match_time = (System.nanoTime() - tmpTime)/1000000.0;

            // sender
            /* create one element */
            MatchSummarySetBuffer = new MatchSendStruct(v);

            final  CountDownLatch summaryLatch = new CountDownLatch(SummaryThreadNum);
            // begin to summarize the match result
            long tmpSummaryBeginTime = System.nanoTime();
            v.EventStartSendTime = System.currentTimeMillis();
            for (int i = 0; i < SummaryThreadNum; i++) {
                Parallel p = new Parallel(i, v, summaryLatch, curSubNum);
                executorSummary.execute(p);
            }
            try {
                summaryLatch.await();
            } catch (Exception e){
                logger.error("Send countdown latch wait error!");
                e.printStackTrace();
            }

            /* finish the summarize match result*/
            long tmpDispatchBeginTime = System.nanoTime();
            float dispatch_size = MatchSummarySetBuffer.subSet.size();
            LastSendThredNum = CurrentSendThreadNum;

            CurrentSendThreadNum = Math.min(
                    Math.max(Math.round(dispatch_size / perThreadExpQueueLength), MIN_SEND_THREAD_NUM),
                    MAX_SEND_THREAD_NUM);
            // TODO: whether use FTSB
//            CurrentSendThreadNum = 10;

            boolean couldIncreasePerThreadQueueLength = !(CurrentSendThreadNum == MIN_SEND_THREAD_NUM);
            boolean couldDecreasePerThreadQueueLength = !(CurrentSendThreadNum == MAX_SEND_THREAD_NUM);

            // dispatch
            try{
                ArrayList<LinkedBlockingQueue<SendPair>> queues = new ArrayList<>();
                for(int i = 0; i < CurrentSendThreadNum; i++){
                    queues.add(new LinkedBlockingQueue<>());
                }

                int count=0;
                for(String subName : MatchSummarySetBuffer.subSet){
                    queues.get(count++ % CurrentSendThreadNum).put(new SendPair(v,subName));
                }

                // TODO: whether use FTSB
                int baseProducer = (int)(Math.random() * MAX_SEND_THREAD_NUM);
//                int baseProducer = 0;

                // begin send
                for(int i = 0; i < CurrentSendThreadNum; i++){
                    MatchResultSendThread t = new MatchResultSendThread(
                            allProducers.get((baseProducer + i) % MAX_SEND_THREAD_NUM),
                            (baseProducer + i) % MAX_SEND_THREAD_NUM,
                            queues.get(i),
                            EventNum);
                    executorSend.execute(t);
                }

            } catch (Exception e){
                logger.error(" Dispatch error");
                e.printStackTrace();
            }

            // summarize this event infomation
            long tmpDispatchEndTime = System.nanoTime();
            logger.info(String.format("Event-%d: sub-num=%d, thread-num=%d, dispatch-size=%d, " +
                            "arrive-delay=%d, match-time=%f, summary-time=%f, dispatch-time=%f",
                    SumEventNum, curSubNum,
                    CurrentMatchThreadNum,
                    (int)dispatch_size,
                    eventArriveTime - v.EventProduceTime,
                    per_match_time,
                    (tmpDispatchBeginTime-tmpSummaryBeginTime)/1000000.0,
                    (tmpDispatchEndTime-tmpDispatchBeginTime)/1000000.0));
            SumDispatchTime += (tmpDispatchEndTime-tmpSummaryBeginTime)/1000000.0;

            // add some statistical vars
            EventNum++;
            SumEventNum++;
            AvgMatchTime = (EventNum > MatchWinSize - MatchWinAvgSize) ?
                    AvgMatchTime + per_match_time : AvgMatchTime;

            AvgKafkaDelayTime = (EventNum > MatchWinSize - MatchWinAvgSize) ?
                    AvgKafkaDelayTime + (eventArriveTime - v.EventProduceTime) : AvgKafkaDelayTime;

            // send state to influxDB
//            influx.matcherInsert(v.EventProduceTime, per_match_time,
//                    (tmpDispatchEndTime-tmpSummaryBeginTime)/1000000.0,0,0);

            // dynamic adjust part
            if(EventNum == MatchWinSize) {
                long tmp = System.nanoTime();

                WindowEndTime = tmp;

                // statistic detain time
                double eventAvgDetainTime = 0;
                for(int eIdx = 0; eIdx < MatchWinSize; eIdx++){
                    // per event 
                    double perEventSumTime = 0;
                    int validThreadCount = 0;
                    for(int tid = 0; tid < MAX_SEND_THREAD_NUM; tid++){
                        perEventSumTime += SendFeedBackTime[eIdx][tid];
                        validThreadCount += (SendFeedBackTime[eIdx][tid] == 0 ? 0 : 1);

                        // clear
                        SendFeedBackTime[eIdx][tid] = 0;
                    }
                    eventAvgDetainTime += perEventSumTime / validThreadCount;
                }
                eventAvgDetainTime = eventAvgDetainTime / MatchWinSize;

                // todo: dynamic change expectation
                // calculate average event delay time
                float tmpDelay = AvgKafkaDelayTime / (float)MatchWinAvgSize;
                MinAvgKafkaDelayTime = Math.max( Math.min(tmpDelay, MinAvgKafkaDelayTime), (float)1.5);

                double tmpVel = WindowEndTime-WindowBeginTime==0 ? 0 : MatchWinSize / ((WindowEndTime-WindowBeginTime)/1000000000.0);
                logger.info(String.format(
                        "************************* Avg delay time:%d / %d = %f, with min=%f, arriving velocity=%f #/s **********************",
                        AvgKafkaDelayTime,
                        MatchWinAvgSize,
                        tmpDelay,
                        MinAvgKafkaDelayTime,
                        tmpVel));

                // put forward
                AvgMatchTime = AvgMatchTime / MatchWinAvgSize;

                // TODO: optimize the send time statistic method, whether use avgDispatchTime?
                AvgDispatchTime = SumDispatchTime / MatchWinSize;
                AvgSendTime = (eventAvgDetainTime - AvgMatchTime - AvgDispatchTime);

                if(tmpVel > (1000.0 / ExpTotalDelay) && eventAvgDetainTime > ExpTotalDelay) {
                    // congested
                    ExpMatchTime = Math.max((int) (ExpMatchTime * (1 - RATE_OF_EVENT_VEL)), 5);
                    ExpSendTime = Math.max((int) (ExpSendTime * (1 - RATE_OF_EVENT_VEL)), 5);

                    addOrMinusPercent = 0.1;

                } else if( tmpVel <= (1000.0 / ExpTotalDelay) || eventAvgDetainTime < ExpTotalDelay * 2 * RATE_OF_EVENT_VEL){
                    ExpMatchTime = Math.min((int) (ExpMatchTime * (1+RATE_OF_EVENT_VEL)) + 1, GlobalExpMatchTimeSet);
                    ExpSendTime = Math.min((int) (ExpSendTime * (1+RATE_OF_EVENT_VEL)),
                            ExpTotalDelay - GlobalExpMatchTimeSet - (int)AvgDispatchTime);

                    addOrMinusPercent = 0.1;
                }
                logger.info(String.format("$$$$$$$$$$$$$ new ExpMatchTime: %d, new ExpSendTime: %d with $$$$$$$$$$",
                        ExpMatchTime,ExpSendTime));

                // adjust match thread number
                int thread_tmp = CurrentMatchThreadNum;

                if(!adjustSwitch && AvgMatchTime > ExpMatchTime * (1 - BASE_RATE)){
                    LastMatchThread = CurrentMatchThreadNum;
                    CurrentMatchThreadNum++;

                    LastMatchTime = AvgMatchTime;
                    adjustSwitch = true;
                }else if(AvgMatchTime > ExpMatchTime * (1 - BASE_RATE)) {
                    if(CurrentMatchThreadNum == 1){
                        LastMatchThread = CurrentMatchThreadNum;
                        CurrentMatchThreadNum = 2;
                        LastMatchTime = AvgMatchTime;
                    }else{
                        double e = (AvgMatchTime - LastMatchTime)/LastMatchTime * CurrentMatchThreadNum/LastMatchThread;
                        if(CurrentMatchThreadNum < LastMatchThread)
                            e = -e;
                        int o = (int)(CurrentMatchThreadNum * (1 - e));
                        o = Math.min(o,MAX_MATCH_THREAD_NUM);
                        if (o > 0 && o != CurrentMatchThreadNum) {
                            LastMatchThread = CurrentMatchThreadNum;
                            LastMatchTime = AvgMatchTime;
                            CurrentMatchThreadNum = o;
                        }
                    }
                }else if(ExpMatchTime * (1 - BASE_RATE) > AvgMatchTime){
                    if(CurrentMatchThreadNum > 1){
                        LastMatchThread = CurrentMatchThreadNum;
                        LastMatchTime = AvgMatchTime;
                        CurrentMatchThreadNum--;
                    }
                }

                // TODO: whether use PhSIH
//                CurrentMatchThreadNum = 10;

                // reset related data structures
                if(CurrentMatchThreadNum != thread_tmp){
                    for(int i = 0;i < CurrentMatchThreadNum; i++){
                        ThreadBucket[stock_id][i].executeNum = 0;
                        for(int j = 0;j<ATTRIBUTE_NUM;j++){
                            ThreadBucket[stock_id][i].bitSet[j] = false;
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
                        for(int j=0;j<CurrentMatchThreadNum;j++){
                            if(min > ThreadBucket[stock_id][j].executeNum){
                                min = ThreadBucket[stock_id][j].executeNum;
                                t = j;
                            }
                        }
                        ThreadBucket[stock_id][t].executeNum+=max;
                        ThreadBucket[stock_id][t].bitSet[n] = true;
                    }
                }


                if (AvgSendTime > ExpSendTime * ( 1 + SEND_BASE_RATE)) {
                    // increase send thread num
                    LastSendTime = AvgSendTime;
                    perThreadExpQueueLength = couldDecreasePerThreadQueueLength ?
                            (int)(perThreadExpQueueLength * (1-addOrMinusPercent)) : perThreadExpQueueLength;
                } else if (AvgSendTime > 0 && AvgSendTime < ExpSendTime * ( 1 - SEND_BASE_RATE)) {
                    // decrease send thread num
                    LastSendTime = AvgSendTime;
                    perThreadExpQueueLength = couldIncreasePerThreadQueueLength ?
                            (int)(perThreadExpQueueLength * (1+addOrMinusPercent)) : perThreadExpQueueLength;
                }

                // print the statistic information
                logger.info(
                        String.format(
                                "Expected_match_time: %d, Aver_match_Time: %f, Last_match_thread: %d, " +
                                        "New_thread_num: %d, Thread_adjust_time: %f, Avg_send_time: %f, " +
                                        "cur_send_thread: %d, new_exp_queue_length: %d, Avg_detain_time: %f, " +
                                        "avg_dispatch_time: %f\n\n",
                                ExpMatchTime,AvgMatchTime,
                                LastMatchThread, CurrentMatchThreadNum,
                                (System.nanoTime() - tmp)/1000000.0, AvgSendTime,
                                CurrentSendThreadNum, perThreadExpQueueLength,
                                eventAvgDetainTime,
                                AvgDispatchTime
                        ));
//                influx.mactcherStateInsert(MAX_MATCH_THREAD_NUM, ExpMatchTime, BASE_RATE,
//                        LastMatchThread,AvgMatchTime,CurrentMatchThreadNum);

                AvgMatchTime = 0;
                EventNum = 0;
                SumDispatchTime = 0;

                AvgKafkaDelayTime = 0;

                WindowBeginTime = System.nanoTime();
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
                executorMatch.shutdown();
                latch.countDown();

                allProducers.clear();
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