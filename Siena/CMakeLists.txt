cmake_minimum_required(VERSION 3.6)
project(Siena)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

set(SOURCE_FILES main.cpp siena.cpp siena.h data_structure.h generator.h generator.cpp printer.cpp printer.h util.h util.cpp)
add_executable(siena ${SOURCE_FILES})