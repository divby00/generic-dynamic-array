#pragma once

#include <stdio.h>

#define __DEBUG__

typedef struct Vector {
    void** data;
    size_t length;
    size_t internal_size;
    void* (*data_memalloc)(void*);
    void (*data_memfree)(void*);
    void (*add)(struct Vector*, void*);
    void (*remove)(struct Vector*, size_t);
    void* (*get)(struct Vector*, size_t);
} Vector;

Vector* vector_init(void* (*)(void*), void (*)(void*));
void vector_quit(Vector*);
