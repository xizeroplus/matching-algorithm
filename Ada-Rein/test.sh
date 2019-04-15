#!/bin/bash

ulimit -s 6553600

g++ -std=c++11 chrono_time.h data_structure.h generator.h generator.cpp printer.h printer.cpp rein.h rein.cpp util.h util.cpp main.cpp -o rein

for ((subs = 200000; subs <= 2000000; subs+=200000)) do
	./rein $subs
	echo "subs $subs done"
done

