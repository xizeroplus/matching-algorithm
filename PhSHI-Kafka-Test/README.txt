This is a test of PhSHI! If the test program is applied in other scenarios, problems may occur. Please use with caution!

Befor test, you need start Kafka. You can get quick start kafka related information on kafka's website https://kafka.apache.org/quickstart. If you don't want to build a kafka cluster, you can test it locally by doing the following:
All test in a linux environment with jdk-11.0.2 or higth version. You can download it from  https://www.oracle.com/technetwork/java/javase/downloads/jdk11-downloads-5066655.html

First you need start zookeeper, Switch your directory to PhSHI-Kafka-Test/kafka_2.11-1.1.0, then start with the following command: ./bin/zookeeper-server-start.sh ./config/zookeeper.properties
Next, satrt Kafka with this command: ./bin/kafka-server-start.sh ./config/server.properties
Then, you need creat topic for test, you can run creatTopic.sh to create topic.

Now, you should have started Kafka. If you are building a kafka cluster yourself, please modify PhSHI-Kafka-Test/resources/config.properties, change KafkaServer too your server address. You can also change the sub file and event file in config.properties.

Switch you directory to PhSHI-Kafka-Test, first ou need creat sub data and event data use creatSub.sh,creatZipfSub.sh and creatEvent.sh. creatZipfSub.sh creat the sub with Zipf distribution.

eventProducer.sh send the evet frome event data. producer.sh send the sub data from sub data for rein and tama. OpIndex use producerOp.sh.

opindex.sh ,tama.sh and rein.sh run three static version of algorithm. First you start algoorithm, then send sub, after that send event.

This test version will test 1,2,4,8,16,32 thread and each mode test 2000 events, please make sure you send the correct number of events. The result data will save in PhSHI-Kafka-Test/resources. The file has one column, save matching time, unit is ms.

The rein-dynamic.sh is the dynamic version of rein. All operations are the same as befor. But the result data has three column, current sub number, matching time and current thread number. Note that every 20 events, we save their average match time and dynamically adjusted time. The unit of time for dynamic adjustment is microseconds.
