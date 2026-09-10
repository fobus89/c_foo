cmake_minimum_required(VERSION 3.22)

project(lesson C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_subdirectory(lib)

add_executable(main src/main.c)
target_link_libraries(main PRIVATE library)

enable_testing()

add_executable(test_library tests/test_library.c)
target_link_libraries(test_library PRIVATE library)

add_test(NAME library_check COMMAND test_library)