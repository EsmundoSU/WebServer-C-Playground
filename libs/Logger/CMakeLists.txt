add_library(logger
        logger.h
        src/logger.c)

target_include_directories(logger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})