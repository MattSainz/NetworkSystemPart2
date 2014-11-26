cmake_minimum_required(VERSION 2.8.4)
project(NetworkSystem_2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

set(SOURCE_FILES
    network_libs/message.h
    network_libs/message.cpp
    include/application.h
    include/network.h
    include/process_per_msg.h
    include/process_per_protocol.h
    include/protocol.h
    src/application.cpp
    src/main.cpp
    src/network.cpp
    src/process_per_msg.cpp
    src/process_per_protocol.cpp
    src/protocol.cpp
    )

add_executable(NetworkSystem_2 ${SOURCE_FILES})