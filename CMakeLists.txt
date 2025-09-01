cmake_minimum_required(VERSION 3.10)
project(TinyKV)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Server executable
add_executable(server
    src/main.cpp
    src/Server.cpp
    src/Store.cpp
)

target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Client executable
add_executable(client
    client/Client.cpp
)

target_include_directories(client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
