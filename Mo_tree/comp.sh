#!/bin/bash
ulimit -s unlimited
g++ -std=c++11 -O3 chrono_time.h data_structure.h generator.h generator.cpp motree.h motree.cpp printer.h printer.cpp util.h util.cpp main.cpp -o Motree
echo "finish compile"
