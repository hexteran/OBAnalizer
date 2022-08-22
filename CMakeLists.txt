
cmake_minimum_required(VERSION 3.22)
set (CMAKE_CXX_STANDARD 17)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../lib)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ../tests/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ../bin)
project(OBAnalizer)
add_library(OBAnalizer SHARED src/OBAnalizer.cpp src/OBAnalizer.h)
set_target_properties(OBAnalizer PROPERTIES PREFIX "")
#add_executable(ex01 tests/source.cpp)
#target_link_libraries(ex01 PUBLIC OBAnalizer)

add_executable(OBExtractor src/BBOExtractor.cpp)
target_link_libraries(OBExtractor PUBLIC OBAnalizer)