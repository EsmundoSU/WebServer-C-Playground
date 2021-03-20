cmake_minimum_required(VERSION 3.5)
project(WebServer_C_Playground)

set(CMAKE_C_STANDARD 99)

# C Compiler Flags
if (MSVC)
    # warning level 4 and
    add_compile_options(/W4)
else ()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
    add_compile_options(-Wno-unknown-pragmas)
endif ()

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_compile_options(-O0)
else ()
    add_compile_options(-O3)
endif ()


# Add C libs/exe
add_subdirectory(libs)
add_subdirectory(core)
add_subdirectory(google_test)

