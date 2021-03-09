cmake_minimum_required(VERSION 3.17)
project(WebServer_C_Playground C)

set(CMAKE_C_STANDARD 99)

# C Compiler Flags
add_compile_options(-O3)
add_compile_options(-Wall -Wextra -Wpedantic -Werror)
add_compile_options(-Wno-unknown-pragmas)

# Add C libs/exe
add_subdirectory(libs)
add_subdirectory(core)

message("C compiler flags: ${CMAKE_C_FLAGS_DEBUG}")

set(CMAKE_VERBOSE_MAKEFILE ON)

