cmake_minimum_required(VERSION 2.8.4)
project(NetworkSystem_2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES src/main.cpp)
add_executable(NetworkSystem_2 ${SOURCE_FILES})