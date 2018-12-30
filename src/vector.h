#pragma once

#include <stdbool.h>
#include <stdio.h>

struct memory_functions {
    void* (*allocate_memory)(void*);
    void (*free_memory)(void*);
};

typedef struct ReducedData {
    void* accumulator;
    void (*free_memory)(void*);
} ReducedData;

typedef struct Vector {
    void** data;
    size_t length;
    size_t internal_size;
    void* (*allocate_memory)(void*);
    void (*free_memory)(void*);
    void (*add)(struct Vector*, void*);
    void (*remove)(struct Vector*, size_t);
    void* (*get)(struct Vector*, size_t);
    struct Vector* (*filter)(struct Vector*, bool (*predicate)(void*), struct memory_functions*);
    struct Vector* (*map)(struct Vector*, struct memory_functions*);
    ReducedData* (*reduce)(struct Vector*, void (*reducer)(void*, void*), void* init_value, struct memory_functions*);
    void* (*find)(struct Vector*, bool (*predicate)(void*));
} Vector;

Vector* vector_init(struct memory_functions*);
void vector_quit(Vector*);
