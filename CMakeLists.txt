cmake_minimum_required(VERSION 3.16)
project(PygmyKV)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# --- Server/Store sources ---
set(SERVER_SOURCES
    src/main.cpp
    src/Server.cpp
    src/Store.cpp
)

set(SERVER_HEADERS
    src/Server.hpp
    src/Store.hpp
)

# --- Client sources ---
set(CLIENT_SOURCES
    client/main.cpp
    client/GUI_Client.cpp
)

set(CLIENT_HEADERS
    client/GUI_Client.hpp
)

# Find Qt6 (Widgets + Network needed)
find_package(Qt6 REQUIRED COMPONENTS Widgets Network)

# --- Build server ---
add_executable(server ${SERVER_SOURCES} ${SERVER_HEADERS})

# --- Build GUI client ---
add_executable(gui_client ${CLIENT_SOURCES} ${CLIENT_HEADERS})

# Link Qt6 modules
target_link_libraries(gui_client PRIVATE Qt6::Widgets Qt6::Network)
