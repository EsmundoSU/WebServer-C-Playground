cmake_minimum_required(VERSION 3.17)
project(WebServer_C_Playground C)

set(CMAKE_C_STANDARD 99)

# Logger include directory
include_directories(libs/Logger)
# TCP Library include directory
include_directories(libs/tcp)

# C Compiler flags
SET(CMAKE_C_FLAGS "-Ofast -Wall -Wextra -Wpedantic")

add_executable(trick-webserver
        core/src/main.c
        libs/Logger/src/logger.c
        libs/tcp/src/tcp.c)