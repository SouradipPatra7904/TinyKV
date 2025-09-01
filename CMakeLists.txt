cmake_minimum_required(VERSION 3.10)
project(TinyKV CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Server executable
add_executable(server
    src/main.cpp
    src/Server.cpp
    src/Store.cpp
)

# Client executable
add_executable(client
    src/Client.cpp
)

target_include_directories(server PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(client PRIVATE ${CMAKE_SOURCE_DIR}/src)

# --- Testing Integration ---
enable_testing()

# Ensure test.sh is executable and in build dir
configure_file(${CMAKE_SOURCE_DIR}/BaseTest.sh ${CMAKE_BINARY_DIR}/BaseTest.sh COPYONLY)

# Register test with CTest
add_test(NAME TinyKV_StoreTest
         COMMAND ${CMAKE_COMMAND} -E env
                 bash ${CMAKE_BINARY_DIR}/BaseTest.sh)
