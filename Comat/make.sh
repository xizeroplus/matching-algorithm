#!/bin/bash
ulimit -s unlimited
g++ -std=c++11 -O3 chrono_time.h data_structure.h generator.h generator.cpp printer.h printer.cpp util.h util.cpp gen.cpp -o Gen
g++ -std=c++11 -O3 chrono_time.h data_structure.h generator.h generator.cpp rein.h rein.cpp tama.h tama.cpp decision.h decision.cpp dnn.h dnn.cpp printer.h printer.cpp util.h util.cpp main.cpp -pthread -lprotobuf -ltensorflow_cc -ltensorflow_framework -o Main
echo finish
