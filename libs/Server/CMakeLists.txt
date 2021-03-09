add_library(server
        server.h
        src/server.c)

target_include_directories(server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(server PRIVATE logger)
target_link_libraries(server PRIVATE socket)

if(WIN32)
    target_link_libraries(server PRIVATE wsock32 ws2_32)
endif()