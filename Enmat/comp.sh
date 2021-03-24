#!/bin/bash
ulimit -s unlimited
g++ -std=c++11 -O3 chrono_time.h data_structure.h generator.h generator.cpp rein.h rein.cpp tama.h tama.cpp enmat.h enmat.cpp enmatp.h enmatp.cpp printer.h printer.cpp util.h util.cpp main.cpp -pthread -o Main
echo finish
