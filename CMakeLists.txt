cmake_minimum_required(VERSION 3.17)
project(WebServer_C_Playground C)

set(CMAKE_C_STANDARD 99)

# Add C libs/exe
add_subdirectory(libs)
add_subdirectory(core)

# C Compiler flags
SET(CMAKE_C_FLAGS "-Ofast -Wall -Wextra -Wpedantic -Werror -Wno-unknown-pragmas")


