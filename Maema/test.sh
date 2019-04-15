#!/bin/bash

ulimit -s 819200

g++ -std=c++11 chrono_time.h data_structure.h generator.h generator.cpp printer.h printer.cpp rein.h rein.cpp util.h util.cpp main.cpp -o maema


for ((subs = 20000; subs <= 200000; subs+=20000)) do
	./maema $subs
	echo "subs $subs done"
done

