cmake_minimum_required(VERSION 3.6)
project(Tama)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -O3")

set(SOURCE_FILES main.cpp data_structure.h generator.h generator.cpp printer.cpp printer.h util.h util.cpp tama.h tama.cpp tama.h tama.cpp)
add_executable(tama ${SOURCE_FILES})