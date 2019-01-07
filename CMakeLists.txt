cmake_minimum_required(VERSION 3.10)
project(generic-dynamic-array C)

set(CMAKE_C_STANDARD 99)
set(DEBUG_PARAMS "-g3 -o0")

add_definitions(${DEBUG_PARAMS})
add_executable(generic-dynamic-array
        src/minunit.h
        src/array.h
        src/array.c
        src/tests.c)
