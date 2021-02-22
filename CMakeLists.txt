cmake_minimum_required(VERSION 3.17)
project(WebServer_C_Playground C)

set(CMAKE_C_STANDARD 99)

# Logger include directory
include_directories(libs/Logger)

add_executable(trick-webserver
        core/src/main.c
        libs/Logger/src/logger.c)