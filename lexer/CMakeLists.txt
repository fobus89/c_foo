# file(GLOB LEXER_SOURCES CONFIGURE_DEPENDS
#     "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
# )
# add_library(lexer ${LEXER_SOURCES})

add_library(lexer 
    lexer.c
    lexer_lib.c
    token.c
)

target_include_directories(lexer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")