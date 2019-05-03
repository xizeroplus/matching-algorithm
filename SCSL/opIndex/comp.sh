#!/bin/bash
ulimit -s unlimited
cd opIndex
g++ -std=c++11 -O3 chrono_time.h data_structure.h generator.h generator.cpp origin_opIndex.h origin_opIndex.cpp opIndex.h opIndex.cpp printer.h printer.cpp util.h util.cpp main.cpp -o opIndex
g++ -std=c++11 -O3 chrono_time.h data_structure.h generator.h generator.cpp origin_opIndex.h origin_opIndex.cpp opIndex.h opIndex.cpp printer.h printer.cpp util.h util.cpp gen.cpp -o Gen
echo "finish compile"
