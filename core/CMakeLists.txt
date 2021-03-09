add_executable(trick-webserver src/main.c)

target_link_libraries(trick-webserver PRIVATE logger)
target_link_libraries(trick-webserver PRIVATE server)

# Copy test HTML files to app directory.
add_custom_command(TARGET trick-webserver PRE_BUILD COMMAND
        ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/html ${CMAKE_CURRENT_BINARY_DIR})