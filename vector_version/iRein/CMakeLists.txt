cmake_minimum_required(VERSION 3.6)
project(Rein_interval)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

set(SOURCE_FILES main.cpp chrono_time.h data_structure.h generator.h generator.cpp printer.cpp printer.h util.h util.cpp rein.h rein.cpp)
add_executable(irein ${SOURCE_FILES})
