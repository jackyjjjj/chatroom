cmake_minimum_required(VERSION 3.15)
project(server)
# 搜索指定目录下源文件
# 包含头文件路径
# file(GLOB SRC_LIST ${PROJECT_SOURCE_DIR}/main.cpp)

# 搜索指定目录下源文件

# file(GLOB SRC_LIST ${PROJECT_SOURCE_DIR}/src/*.cpp)
# include_directories(${PROJECT_SOURCE_DIR}/include/*.h)
# add_library(chatroom STATIC ${SRC_LIST})
# set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 包含静态库路径
# 链接静态库

file(GLOB SRC_LIST ${PROJECT_SOURCE_DIR}/main.cpp)
link_directories(${PROJECT_SOURCE_DIR}/lib/)
link_libraries(chatroom)
add_executable(room ${SRC_LIST})

