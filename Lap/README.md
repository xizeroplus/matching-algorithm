# Lap
This is the source code of Lap!
Please use jdk-11.0.2 or higher version!
Kafka version is 2.11-1.1.0!

This code is just uesed for test, if the code is applied in other scenarios, problems may occur. Please use with caution!

## directories
1. The Client folder contains the generation, sending, and receiving of subscriptions. 
2. The core folder contains static versions of rein, tama and opindex and a Lap imeplementation(PhSIH implemented with REIN). 
3. The EventSender folder contains the sending of events.

## usage
modify pom.xml and use maven to build your target jar application. Deploy a Kafka cluster and then configure then config files in **resource** directory. 
1. Run Lap module(src/main/java/core/Lap_REIN.java) 
2. Run subscription(src/main/java/Client/subProducer.java) module to generate subs
3. Run event module(src/main/java/EventSender/EventProducer.java) module to generate events

then the Lap work to work. For testing consumer infomation, you can build consumer(src/main/java/Client/Consumer.java) appication to verify.