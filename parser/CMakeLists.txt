add_subdirectory(
    "${CMAKE_CURRENT_SOURCE_DIR}/../lexer"
    "${CMAKE_CURRENT_BINARY_DIR}/lexer"
)

add_library(parser parser.c)

target_link_libraries(parser PUBLIC lexer)

target_include_directories(parser PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)