cmake_minimum_required(VERSION 3.15)
project(client)

file(GLOB SRC ${PROJECT_SOURCE_DIR}/client.cpp)
include_directories(${PROJECT_SOURCE_DIR}/client.h)
add_executable(client ${SRC})