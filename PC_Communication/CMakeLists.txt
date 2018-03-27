cmake_minimum_required(VERSION 3.8)
project(PC_Communication)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        board_info.h
        CommunicateNode.cpp
        CommunicateNode.h
        judgement_info.h
        main.cpp
        protocol.cpp
        protocol.h)

add_executable(PC_Communication ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(PC_Communication boost_system)