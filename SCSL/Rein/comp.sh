#!/bin/bash
ulimit -s unlimited
cd Rein
g++ -std=c++11 -O3 chrono_time.h data_structure.h generator.h generator.cpp oldrein.h oldrein.cpp rein.h rein.cpp printer.h printer.cpp util.h util.cpp gen.cpp -o Gen
g++ -std=c++11 -O3 chrono_time.h data_structure.h generator.h generator.cpp oldrein.h oldrein.cpp rein.h rein.cpp printer.h printer.cpp util.h util.cpp main.cpp -o Rein
echo "finish compile"
