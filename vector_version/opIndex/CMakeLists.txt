cmake_minimum_required(VERSION 3.6)
project(opIndex)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

set(SOURCE_FILES main.cpp opIndex.h data_structure.h generator.h generator.cpp chrono_time.h opIndex.cpp printer.h printer.cpp util.cpp util.h)
add_executable(opindex ${SOURCE_FILES})