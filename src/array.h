#pragma once

#include <stdbool.h>
#include <stdio.h>

typedef struct ArrayElement {
    void *data;
    void *accumulator;
    void (*free_memory)(struct ArrayElement *);
} ArrayElement;

struct memory_functions {
    ArrayElement *(*allocate_memory)(void *);
    void (*free_memory)(ArrayElement *);
    ArrayElement *(*map_allocate_memory)(void *);
};

typedef struct Array {
    ArrayElement **elements;
    size_t length;
    size_t internal_size;
    ArrayElement *(*allocate_memory)(void *);
    ArrayElement *(*map_allocate_memory)(void *);
    void (*free_memory)(ArrayElement *);
    void (*add)(struct Array *, void *);
    void (*remove)(struct Array *, size_t);
    ArrayElement *(*get)(struct Array *, size_t);
    struct Array *(*filter)(struct Array *, bool (*predicate)(ArrayElement *), void *(*get_data)(ArrayElement *));
    struct Array *(*map)(struct Array *, struct memory_functions *);
    ArrayElement * (*reduce)(struct Array *, void (*reducer)(ArrayElement *, ArrayElement *), void *init_value, struct memory_functions *);
    ArrayElement *(*find)(struct Array *, bool (*predicate)(ArrayElement *));
} Array;

Array *array_init(struct memory_functions *);
Array *array_quit(Array *);
