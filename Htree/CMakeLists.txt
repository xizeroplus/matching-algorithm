cmake_minimum_required(VERSION 3.6)
project(Htree)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

set(SOURCE_FILES main.cpp data_structure.h generator.h generator.cpp printer.cpp printer.h util.h util.cpp htree.h htree.cpp)
add_executable(htree ${SOURCE_FILES})