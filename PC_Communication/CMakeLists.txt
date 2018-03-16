cmake_minimum_required(VERSION 3.5)
project(PC_Communication)

set(CMAKE_CXX_STANDARD 11)
set(SRC_FILES
        CommunicateNode.cpp
        CommunicateNode.h
           protocol.cpp
            protocol.h
        board_info.h
        judgement_info.h
        )
add_executable(PC_Communication main.cpp ${SRC_FILES})
TARGET_LINK_LIBRARIES(PC_Communication boost_system)
