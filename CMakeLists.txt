cmake_minimum_required(VERSION 3.13)
project(redis-starter-cpp)
set(CMAKE_CXX_STANDARD 20) # Enable the C++20 standard

set(SOURCE_FILES src/Server.cpp
        src/RespParser.cpp
        src/RespParser.h
        src/RedisApp.cpp
        src/RedisApp.h
        src/RespSerializer.cpp
        src/RespSerializer.h
        src/utils.h
        src/utils.cpp)

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

add_executable(server ${SOURCE_FILES})

TARGET_LINK_LIBRARIES(server
        pthread
        stdc++
)