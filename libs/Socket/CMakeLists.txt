project(socket)

set(PRIVATE_HEADERS include/socket-unix.h)
set(PUBLIC_HEADERS socket.h)
if (WIN32)
    set(SOURCE_FILES src/socket_win32.c)
else ()
    set(SOURCE_FILES src/socket_unix.c)
endif ()


add_library(${PROJECT_NAME} ${PRIVATE_HEADERS} ${PUBLIC_HEADERS} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add libraries
target_link_libraries(${PROJECT_NAME} PRIVATE logger)

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE wsock32 ws2_32)
endif ()