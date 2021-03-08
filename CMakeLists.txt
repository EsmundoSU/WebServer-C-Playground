cmake_minimum_required(VERSION 3.17)
project(WebServer_C_Playground C)

set(CMAKE_C_STANDARD 99)

message(CMAKE_BUILD_FILES_DIRECTORY)

# Configuration
include_directories(core)
include_directories(core/include)
# Logger include directory
include_directories(libs/Logger)
# Server staff include directory
include_directories(libs/Server)
include_directories(libs/Socket)

configure_file(html/index.html index.html COPYONLY)

# C Compiler flags
SET(CMAKE_C_FLAGS "-Ofast -Wall -Wextra -Wpedantic -Werror -Wno-unknown-pragmas")

add_executable(trick-webserver
        core/src/main.c
        libs/Logger/src/logger.c
        libs/Socket/src/socket.c
        libs/Server/src/server.c)
if(WIN32)
    target_link_libraries(trick-webserver wsock32 ws2_32)
endif()