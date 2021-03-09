add_library(socket
        socket.h
        src/socket.c)

target_include_directories(socket PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(socket PRIVATE logger)

if(WIN32)
    target_link_libraries(socket PRIVATE wsock32 ws2_32)
endif()