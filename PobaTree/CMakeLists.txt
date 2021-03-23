cmake_minimum_required(VERSION 3.5)
project(Rein_interval)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -O3")

set(SOURCE_FILES main.cpp chrono_time.h data_structure.h 
                    generator.h generator.cpp printer.cpp 
                    printer.h util.h util.cpp PobaTree.h PobaTree.cpp)
add_executable(PobaTree ${SOURCE_FILES})

set(SOURCE_FILES1 helper.cc  data_structure.h 
                    generator.h generator.cpp printer.cpp 
                    printer.h util.h util.cpp )
add_executable(dataFileGenerator ${SOURCE_FILES1})
