cmake_minimum_required(VERSION 3.10)
project(generic-dynamic-array C)

set(CMAKE_C_STANDARD 99)
set(DEBUG "-g3 -o0 -D__DEBUG__")
add_definitions(${DEBUG})

add_executable(generic-dynamic-array
        src/minunit.h
        src/vector.h
        src/vector.c
        src/main.c)