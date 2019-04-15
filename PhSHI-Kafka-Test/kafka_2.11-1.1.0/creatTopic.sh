#!/bin/sh
zookeeperServer = "localhost:2181"

./bin/kafka-topics.sh \
--create \
--zookeeper $zookeeperServer \
--replication-factor 1 \
--partitions 10 \
--topic Sub &&

./bin/kafka-topics.sh \
--create \
--zookeeper $zookeeperServer \
--replication-factor 1 \
--partitions 10 \
--topic Event &&

./bin/kafka-topics.sh \
--create \
--zookeeper $zookeeperServer \
--replication-factor 1 \
--partitions 10 \
--topic Client1 &&

echoo "done"
