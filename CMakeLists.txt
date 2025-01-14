cmake_minimum_required(VERSION 3.22)
project(safecpp)

set(CMAKE_CXX_STANDARD 23)

# Add executable files
add_executable(safecpp
        main.cpp
)
target_include_directories(safecpp PUBLIC include)

# Compiler static analysis flags
target_compile_options(safecpp PRIVATE -Wall -Wextra -Wshadow -Wconversion -Wpedantic -Werror)
target_compile_options(safecpp PRIVATE -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Woverloaded-virtual -Wunused -Wsign-conversion -Wnull-dereference -Wdouble-promotion -Wformat=2 -Wimplicit-fallthrough -Wno-narrowing)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message("Using GCC Compiler")
    target_compile_options(safecpp PRIVATE -fanalyzer -Wlogical-op)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message("Using Clang Compiler")
    target_compile_options(safecpp PRIVATE -Wlong-long)
endif ()

# Add GTest
include(setupGTest.cmake)
