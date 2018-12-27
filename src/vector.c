#include <stdlib.h>
#include <string.h>
#include "vector.h"

static const size_t VECTOR_INITIAL_SIZE = 10;

static void vector_add(Vector* this, void* data)
{
    if (this->length == this->internal_size) {
        this->internal_size *= 2;
        this->data = realloc(this->data, this->internal_size * sizeof(void*));
    }
    void* output = this->data_memalloc(data);
    this->data[this->length] = output;
    this->length++;
}

static void vector_remove(Vector* this, size_t position)
{
#ifdef __DEBUG__
    if (position < this->length && position >= 0) {
#endif
        this->internal_size--;
        this->length--;
        size_t first_offset = position * sizeof(void*);
        size_t second_offset = (this->internal_size - position) * sizeof(void*);
        void** data = calloc(sizeof(void*), this->internal_size);
        memcpy(data, this->data, first_offset);
        memcpy(data + position, this->data + position + 1, second_offset);
        this->data_memfree(this->data[position]);
        free(this->data);
        this->data = data;
#ifdef __DEBUG__
    } else {
        fprintf(stderr, "Out of bounds error! Trying to remove vector element at position %ld\n", position);
    }
#endif
}

static void* vector_get(Vector* this, size_t position)
{
#ifdef __DEBUG__
    if (position < this->length && position >= 0) {
#endif
        return this->data[position];
#ifdef __DEBUG__
    } else {
        fprintf(stderr, "Out of bounds error! trying to get vector element at position %ld\n", position);
        return NULL;
    }
#endif
}

Vector* vector_init(void* (*data_memalloc)(void*), void (*data_memfree)(void* data))
{
    Vector* vector = calloc(sizeof(struct Vector), 1);
    vector->data_memalloc = data_memalloc;
    vector->data_memfree = data_memfree;
    vector->length = 0;
    vector->internal_size = VECTOR_INITIAL_SIZE;
    vector->data = calloc(sizeof(void*), vector->internal_size);
    vector->add = vector_add;
    vector->remove = vector_remove;
    vector->get = vector_get;
    return vector;
}

void vector_quit(Vector* vector)
{
    if (vector) {
        if (vector->data) {
            for (size_t i = 0; i < vector->internal_size; i++) {
                vector->data_memfree(vector->data[i]);
            }
            free(vector->data);
            vector->data = NULL;
        }
        free(vector);
        vector = NULL;
    }
}
