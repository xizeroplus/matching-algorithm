cmake_minimum_required(VERSION 3.6)
project(AWBTree)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")

set(SOURCE_FILES main.cpp chrono_time.h data_structure.h bplustree.h
                    generator.h generator.cpp printer.cpp
                    printer.h util.h util.cpp AWBTree.h AWBTree.cpp)
add_executable(AWBTree ${SOURCE_FILES})

