cmake_minimum_required(VERSION 3.22)

project(lesson C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_subdirectory(parser)

add_executable(main src/main.c)
target_link_libraries(main PRIVATE parser)

# enable_testing()
# add_subdirectory(tests)