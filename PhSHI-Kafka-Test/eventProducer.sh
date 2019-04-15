#!/bin/sh
runclass=EventSender.EventProducer
classpath=./lib/*:./bin
java -cp $classpath $runclass
