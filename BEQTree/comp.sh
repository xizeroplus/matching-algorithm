#!/bin/bash
ulimit -s unlimited
g++ -std=c++11 -O3 chrono_time.h data_structure.h generator.h generator.cpp beq.h beq.cpp printer.h printer.cpp util.h util.cpp main.cpp -o Beq
echo "finish compile"
