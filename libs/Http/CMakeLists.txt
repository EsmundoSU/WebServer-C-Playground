# List headers files used for this library only.
set(PRIVATE_HEADERS include/http-requestmethod.h include/http-response.h)
# List of public header interfaces of this library.
set(PUBLIC_HEADERS http.h)
# List of source files
set(SOURCE_FILES src/http.c src/http-requestmethod.c)

# Create target library
add_library(http ${PRIVATE_HEADERS} ${SOURCE_FILES} ${PUBLIC_HEADERS} )

target_include_directories(http PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(http PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/src)