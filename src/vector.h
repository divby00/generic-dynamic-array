#pragma once

#include <stdbool.h>
#include <stdio.h>

typedef struct VectorElement {
    void *data;
    void *accumulator;
    void (*free_memory)(struct VectorElement *);
} VectorElement;

struct memory_functions {
    VectorElement *(*allocate_memory)(void *);
    void (*free_memory)(VectorElement *);
    VectorElement *(*map_allocate_memory)(void *);
};

typedef struct Vector {
    VectorElement **elements;
    size_t length;
    size_t internal_size;
    VectorElement *(*allocate_memory)(void *);
    VectorElement *(*map_allocate_memory)(void *);
    void (*free_memory)(VectorElement *);
    void (*add)(struct Vector *, void *);
    void (*remove)(struct Vector *, size_t);
    VectorElement *(*get)(struct Vector *, size_t);
    struct Vector *(*filter)(struct Vector *, bool (*predicate)(VectorElement *), void *(*get_data)(VectorElement *));
    struct Vector *(*map)(struct Vector *, struct memory_functions *);
    VectorElement * (*reduce)(struct Vector *, void (*reducer)(void *, void *), void *init_value, struct memory_functions *);
    VectorElement *(*find)(struct Vector *, bool (*predicate)(VectorElement *));
} Vector;

Vector *vector_init(struct memory_functions *);
Vector* vector_quit(Vector *);
