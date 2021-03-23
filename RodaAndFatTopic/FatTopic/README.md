## kafka source code midification
 
### `kafka-2.2.0-src/core/src/main/scala/kafka/server/KafkaApis.scala`文件

1. 新增
```
531    // VIRTUAL TOPIC: get the field from FetchRequest
532    val subscriberName:String = fetchRequest.getSubscriberName
533    val isVirtual: Boolean = fetchRequest.getIsVirtual
```
2. 新增（658行为原始代码）
```
648          // VIRTUAL TOPIC: if FetchRequest is pull messages from virtual topic, check subscriberName in unconverted records
649          if(isVirtual){
650            val memRec = RecordsUtil.convertToMemRecords(unconvertedPartitionData.records, subscriberName)
651            val newUnconvertedData = new FetchResponse.PartitionData(unconvertedPartitionData.error,
652              unconvertedPartitionData.highWatermark,
653              unconvertedPartitionData.lastStableOffset, unconvertedPartitionData.logStartOffset,
654              unconvertedPartitionData.abortedTransactions, memRec)
655
656            convertedData.put(tp, maybeConvertFetchedData(tp, newUnconvertedData))
657          } else {
658            convertedData.put(tp, maybeConvertFetchedData(tp, unconvertedPartitionData))
659          }
```

### `kafka-2.2.0-src/clients/src/main/java/org/apache/kafka/common/requests/FetchResponse.java` 文件

1. 新增


```
47 import static org.apache.kafka.common.protocol.types.Type.*;
```

2. 新增
```
186    // VIRTUAL TOPIC: V11 Used for virtual topic
187    private static final Schema FETCH_RESPONSE_V11 = FETCH_RESPONSE_V10;
```

3. 修改: `schemaVersions`新增`FETCH_RESPONSE_V11`
```
189    public static Schema[] schemaVersions() {
190        return new Schema[] {FETCH_RESPONSE_V0, FETCH_RESPONSE_V1, FETCH_RESPONSE_V2,
191            FETCH_RESPONSE_V3, FETCH_RESPONSE_V4, FETCH_RESPONSE_V5, FETCH_RESPONSE_V6,
192            FETCH_RESPONSE_V7, FETCH_RESPONSE_V8, FETCH_RESPONSE_V9, FETCH_RESPONSE_V10,
193                FETCH_RESPONSE_V11 }; // VIRTUAL TOPIC: add version 11
194    }
```

### `kafka-2.2.0-src/clients/src/main/java/org/apache/kafka/common/requests/FetchRequest.java` 文件

1. 新增197行到217行
```
    // VIRTUAL TOPIC: V11 Used for virtual topic
    private static final Field.NullableStr SUBSCRIBER_NAME = new Field.NullableStr("subscriber_name",
            "Virtual topic used to show the subscriber's name/id");
    private static final Field.Bool IS_VIRTUAL = new Field.Bool("is_virtual",
            "Virtual topic used to show the subscriber wants to consume messages in virtual topic");
    private static final Schema FETCH_REQUEST_V11 = new Schema(
            REPLICA_ID,
            MAX_WAIT_TIME,
            MIN_BYTES,
            MAX_BYTES,
            ISOLATION_LEVEL,
            SESSION_ID,
            SESSION_EPOCH,
            FETCH_REQUEST_TOPIC_V9,
            FORGOTTEN_TOPIC_DATA_V7,
            SUBSCRIBER_NAME,
            IS_VIRTUAL);

    // VIRTUAL TOPIC: object variables to store new fields
    private final String subscriberName;
    private final boolean isVirtual ;
```

2. 修改： `schemaVersions`增加`FETCH_REQUEST_V11`
```
    public static Schema[] schemaVersions() {
        return new Schema[]{FETCH_REQUEST_V0, FETCH_REQUEST_V1, FETCH_REQUEST_V2, FETCH_REQUEST_V3, FETCH_REQUEST_V4,
            FETCH_REQUEST_V5, FETCH_REQUEST_V6, FETCH_REQUEST_V7, FETCH_REQUEST_V8, FETCH_REQUEST_V9,
            FETCH_REQUEST_V10, FETCH_REQUEST_V11}; // VIRTUAL TOPIC: add version 11
    }
```

3. 新增316～345行
```
        // VIRTUAL TOPIC: add two variables to  store new fields
        private String subscriberName;
        private boolean isVirtual;
        /*
        VIRTUAL TOPIC: function call for virtual topic consumers to use,
         reference function: public static Builder forConsumer(int maxWait, int minBytes, Map<TopicPartition, PartitionData> fetchData)
         */
        public static Builder forConsumer(int maxWait, int minBytes, Map<TopicPartition, PartitionData> fetchData,
                                          String subscriberName, boolean isVirtual) {
            return new Builder(ApiKeys.FETCH.oldestVersion(), ApiKeys.FETCH.latestVersion(),
                    CONSUMER_REPLICA_ID, maxWait, minBytes, fetchData,
                    subscriberName, isVirtual);
        }
        /*
        VIRTUAL TOPIC: function call for virtual topic consumers to use,
         reference function: public Builder(short minVersion, short maxVersion, int replicaId, int maxWait, int minBytes,
                        Map<TopicPartition, PartitionData> fetchData)
         */
        public Builder(short minVersion, short maxVersion, int replicaId, int maxWait, int minBytes,
                       Map<TopicPartition, PartitionData> fetchData,
                       String subscriberName, boolean isVirtual) {
            super(ApiKeys.FETCH, minVersion, maxVersion);
            this.replicaId = replicaId;
            this.maxWait = maxWait;
            this.minBytes = minBytes;
            this.fetchData = fetchData;

            this.subscriberName = subscriberName;
            this.isVirtual = isVirtual;
        }
```

4. 新增400～404行
```
            // VIRTUAL TOPIC: if version 11, call the latest construction function
            if(version == 11){
                return new FetchRequest(version, replicaId, maxWait, minBytes, maxBytes, fetchData,
                        isolationLevel, toForget, metadata, subscriberName, isVirtual);
            }
```

5. 新增422、423行

```
        append(", subscriberName=").append(subscriberName).
        append(", isVirtual=").append(isVirtual). // VIRTUAL TOPIC: add new fields toString
```

6. 新增443、444行

```
        // VIRTUAL TOPIC: assign values to new added variables
        this.subscriberName = "";
        this.isVirtual = false;
```

7. 新增447～470行

```
    /*
    VIRTUAL TOPIC: construction function for virtual topic consumer builder to use,
     reference function: private FetchRequest(short version, int replicaId, int maxWait, int minBytes, int maxBytes,
                             Map<TopicPartition, PartitionData> fetchData, IsolationLevel isolationLevel,
                             List<TopicPartition> toForget, FetchMetadata metadata)
     */
    private FetchRequest(short version, int replicaId, int maxWait, int minBytes, int maxBytes,
                         Map<TopicPartition, PartitionData> fetchData, IsolationLevel isolationLevel,
                         List<TopicPartition> toForget, FetchMetadata metadata,
                         String subscriberName, boolean isVirtual) {
        super(ApiKeys.FETCH, version);
        this.replicaId = replicaId;
        this.maxWait = maxWait;
        this.minBytes = minBytes;
        this.maxBytes = maxBytes;
        this.fetchData = fetchData;
        this.isolationLevel = isolationLevel;
        this.toForget = toForget;
        this.metadata = metadata;

        // VIRTUAL TOPIC: assign values to new added variables
        this.subscriberName = subscriberName;
        this.isVirtual = isVirtual;
    }
```

8. 新增479～487行
```
        // VIRTUAL TOPIC: assign values to new added variables
        if(struct.hasField(SUBSCRIBER_NAME))
            subscriberName = struct.get(SUBSCRIBER_NAME);
        else
            subscriberName = "";
        if(struct.hasField(IS_VIRTUAL))
            isVirtual = struct.get(IS_VIRTUAL);
        else
            isVirtual = false;
```

9. 新增577～585行
```
    // VIRTUAL TOPIC: get method to read subscriberName field
    public String getSubscriberName(){
        return subscriberName;
    }

    // VIRTUAL TOPIC: get method to read isVirtual field
    public boolean getIsVirtual(){
        return isVirtual;
    }
```

10. 新增609～611行
```
        // VIRTUAL TOPIC: set value in out put struct
        struct.setIfExists(SUBSCRIBER_NAME, subscriberName);
        struct.setIfExists(IS_VIRTUAL, isVirtual);
```

### `kafka-2.2.0-src/clients/src/main/java/org/apache/kafka/common/record/RecordsUtil.java` 文件

1. 新增36～30行

```
import java.nio.CharBuffer;
import java.nio.charset.Charset;
import java.util.Iterator;
import java.nio.charset.StandardCharsets;
```

2. 新增129～166行`converToMemRecords`函数

```
    /*
    VIRTUAL TOPIC: a util method to convert read log file records to MemoryRecords,
        broker will check whether subscriberName is in message's key
     */
    public static Records convertToMemRecords(Records records, String subscriberName) {
        ByteBuffer buffer = ByteBuffer.allocate(records.sizeInBytes());
        MemoryRecordsBuilder builder = MemoryRecords.builder(buffer, RecordBatch.MAGIC_VALUE_V2,
                CompressionType.NONE, TimestampType.CREATE_TIME,
                0, 0);

        Iterator<Record> rsIt = records.records().iterator();
        Charset charset = StandardCharsets.UTF_8;
        while(rsIt.hasNext()){
            Record curIterator = rsIt.next();
            if(curIterator.hasKey()){
                CharBuffer charBuffer = charset.decode(curIterator.key());
                String virtualMessageKey = charBuffer.toString();

                if(virtualMessageKey.contains("`"+subscriberName+"`")){
                    builder.appendWithOffset(curIterator.offset(), curIterator.timestamp(),
                            curIterator.key(), curIterator.value());
                } else {
                    builder.appendWithOffset(curIterator.offset(), curIterator.timestamp(),
                            curIterator.key(), null);
                    // System.out.printf("message with offset %s does not contain %s, response null\n",
                    //        records.toString(), subscriberName);
                }
            } else {
                builder.appendWithOffset(curIterator.offset(), curIterator.timestamp(),
                        curIterator.key(), null);
                System.out.printf("message with offset %s does not have key, response null\n",
                        records.toString());
            }
        }

        builder.close();
        return builder.build();
    }
```

### `kafka-2.2.0-src/clients/src/main/java/org/apache/kafka/clients/consumer/internals/Fetcher.java` 文件

1. 新增269～336行，`sendFetchesForVirtualTopic`函数
```
    /*
    VIRTUAL TOPIC: function call for virtual topic consumers to construct and send FetchRequest to broker,
     reference function: public synchronized int sendFetches()
     */
    public synchronized int sendFetchesForVirtualTopic(String subscriberName) {
        Map<Node, FetchSessionHandler.FetchRequestData> fetchRequestMap = prepareFetchRequests();
        for (Map.Entry<Node, FetchSessionHandler.FetchRequestData> entry : fetchRequestMap.entrySet()) {
            final Node fetchTarget = entry.getKey();
            final FetchSessionHandler.FetchRequestData data = entry.getValue();

            // VIRTUAL TOPIC: call new FetchRequest.forConsumer
            final FetchRequest.Builder request = FetchRequest.Builder
                    .forConsumer(this.maxWaitMs, this.minBytes, data.toSend(), subscriberName, true)
                    .isolationLevel(isolationLevel)
                    .setMaxBytes(this.maxBytes)
                    .metadata(data.metadata())
                    .toForget(data.toForget());
            if (log.isDebugEnabled()) {
                log.debug("Sending {} {} to broker {}", isolationLevel, data.toString(), fetchTarget);
            }
            client.send(fetchTarget, request)
                    .addListener(new RequestFutureListener<ClientResponse>() {
                        @Override
                        public void onSuccess(ClientResponse resp) {
                            synchronized (Fetcher.this) {
                                @SuppressWarnings("unchecked")
                                FetchResponse<Records> response = (FetchResponse<Records>) resp.responseBody();
                                FetchSessionHandler handler = sessionHandler(fetchTarget.id());
                                if (handler == null) {
                                    log.error("Unable to find FetchSessionHandler for node {}. Ignoring fetch response.",
                                            fetchTarget.id());
                                    return;
                                }
                                if (!handler.handleResponse(response)) {
                                    return;
                                }

                                Set<TopicPartition> partitions = new HashSet<>(response.responseData().keySet());
                                FetchResponseMetricAggregator metricAggregator = new FetchResponseMetricAggregator(sensors, partitions);

                                for (Map.Entry<TopicPartition, FetchResponse.PartitionData<Records>> entry : response.responseData().entrySet()) {
                                    TopicPartition partition = entry.getKey();
                                    long fetchOffset = data.sessionPartitions().get(partition).fetchOffset;
                                    FetchResponse.PartitionData<Records> fetchData = entry.getValue();

                                    log.debug("Fetch {} at offset {} for partition {} returned fetch data {}",
                                            isolationLevel, fetchOffset, partition, fetchData);
                                    completedFetches.add(new CompletedFetch(partition, fetchOffset, fetchData, metricAggregator,
                                            resp.requestHeader().apiVersion()));
                                }

                                sensors.fetchLatency.record(resp.requestLatencyMs());
                            }
                        }

                        @Override
                        public void onFailure(RuntimeException e) {
                            synchronized (Fetcher.this) {
                                FetchSessionHandler handler = sessionHandler(fetchTarget.id());
                                if (handler != null) {
                                    handler.handleError(e);
                                }
                            }
                        }
                    });
        }
        return fetchRequestMap.size();
    }
```

2. 新增601～667行，`fetchedRecordsFromVirtualTopic`函数
```
    /*
    VIRTUAL TOPIC: function call for virtual topic consumers to get records results in broker's respond,
     reference function: public Map<TopicPartition, List<ConsumerRecord<K, V>>> fetchedRecords()
     */
    public Map<TopicPartition, List<ConsumerRecord<K, V>>> fetchedRecordsFromVirtualTopic() {
        Map<TopicPartition, List<ConsumerRecord<K, V>>> fetched = new HashMap<>();
        int recordsRemaining = maxPollRecords;

        try {
            while (recordsRemaining > 0) {
                if (nextInLineRecords == null || nextInLineRecords.isFetched) {
                    CompletedFetch completedFetch = completedFetches.peek();
                    if (completedFetch == null) break;

                    try {
                        nextInLineRecords = parseCompletedFetch(completedFetch);
                    } catch (Exception e) {
                        // Remove a completedFetch upon a parse with exception if (1) it contains no records, and
                        // (2) there are no fetched records with actual content preceding this exception.
                        // The first condition ensures that the completedFetches is not stuck with the same completedFetch
                        // in cases such as the TopicAuthorizationException, and the second condition ensures that no
                        // potential data loss due to an exception in a following record.
                        FetchResponse.PartitionData partition = completedFetch.partitionData;
                        if (fetched.isEmpty() && (partition.records == null || partition.records.sizeInBytes() == 0)) {
                            completedFetches.poll();
                        }
                        throw e;
                    }
                    completedFetches.poll();
                } else {
                    List<ConsumerRecord<K, V>> recordsFromVirtual = fetchRecords(nextInLineRecords, recordsRemaining);

                    // VIRTUAL TOPIC: modified codes for virtual topic from fetchedRecords
                    //      if there is none value in record, the record is virtual record
                    List<ConsumerRecord<K, V>> records = new ArrayList<>();
                    for(ConsumerRecord<K,V> r: recordsFromVirtual){
                        if(r.value() != null){
                            records.add(r);
                        }
                        /* else {
                            System.out.printf("records(offset=%d) is null, ignore it\n", r.offset());
                        } */
                    }
                    TopicPartition partition = nextInLineRecords.partition;
                    if (!records.isEmpty()) {
                        List<ConsumerRecord<K, V>> currentRecords = fetched.get(partition);
                        if (currentRecords == null) {
                            fetched.put(partition, records);
                        } else {
                            // this case shouldn't usually happen because we only send one fetch at a time per partition,
                            // but it might conceivably happen in some rare cases (such as partition leader changes).
                            // we have to copy to a new list because the old one may be immutable
                            List<ConsumerRecord<K, V>> newRecords = new ArrayList<>(records.size() + currentRecords.size());
                            newRecords.addAll(currentRecords);
                            newRecords.addAll(records);
                            fetched.put(partition, newRecords);
                        }
                        recordsRemaining -= records.size();
                    }
                }
            }
        } catch (KafkaException e) {
            if (fetched.isEmpty())
                throw e;
        }
        return fetched;
    }

```

### `kafka-2.2.0-src/clients/src/main/java/org/apache/kafka/clients/consumer/VirtualTopicKafkaConsumer.java` 文件
基于KafkaConsumer.java新增的文件


### `/Users/LeroyXu/Codes/roda_monitor/kafka-2.2.0-src/clients/src/main/java/org/apache/kafka/clients/producer/VirtualTopicKafkaProducer.java`
基于KafkaProducer.java新增的文件
