#include <stdlib.h>
#include <string.h>
#include "vector.h"

static const size_t VECTOR_INITIAL_SIZE = 2;

static void double_data_size(Vector *this) {
    this->internal_size += this->internal_size / 2;
    this->elements = realloc(this->elements, this->internal_size * sizeof(struct VectorElement *));
}

static void vector_add(Vector *this, void *data) {
    if (this->length == this->internal_size) {
        double_data_size(this);
    }
    VectorElement *element = this->allocate_memory(data);
    element->free_memory = this->free_memory;
    this->elements[this->length] = element;
    this->length++;
}

static void vector_map_add(Vector *this, VectorElement *element) {
    if (this->length == this->internal_size) {
        double_data_size(this);
    }
    element->free_memory = this->free_memory;
    this->elements[this->length] = element;
    this->length++;
}

static void vector_remove(Vector *this, size_t position) {
#ifdef __DEBUG__
    if (position < this->length && position >= 0) {
#endif
        this->internal_size--;
        this->length--;
        size_t first_offset = position * sizeof(struct VectorElement *);
        size_t second_offset = (this->internal_size - position) * sizeof(struct VectorElement *);
        VectorElement **data = calloc(sizeof(struct VectorElement *), this->internal_size);
        memcpy(data, this->elements, first_offset);
        memcpy(data + position, this->elements + position + 1, second_offset);
        VectorElement *element = this->elements[position];
        element->free_memory(element);
        free(this->elements);
        this->elements = data;
#ifdef __DEBUG__
    } else {
        fprintf(stderr, "Out of bounds error! Trying to remove vector element at position %ld\n", position);
    }
#endif
}

static VectorElement *vector_get(Vector *this, size_t position) {
#ifdef __DEBUG__
    if (position < this->length && position >= 0) {
#endif
        return this->elements[position];
#ifdef __DEBUG__
    } else {
        fprintf(stderr, "Out of bounds error! trying to get vector element at position %ld\n", position);
        return NULL;
    }
#endif
}

static Vector *vector_filter(Vector *this, bool (*predicate)(VectorElement *), void *(*get_data)(VectorElement *)) {
#ifdef __DEBUG__
    if (this->allocate_memory == NULL || this->free_memory == NULL) {
        fprintf(stderr, "You can't use vector filter without a memory allocation / free function\n");
        return NULL;
    }
#endif
    struct memory_functions memory_functions = {
            .allocate_memory = this->allocate_memory,
            .free_memory = this->free_memory,
            .map_allocate_memory = this->map_allocate_memory
    };
    Vector *vector = vector_init(&memory_functions);
    for (size_t i = 0; i < this->length; i++) {
        VectorElement *element = this->get(this, i);
        if (predicate(element)) {
            vector->add(vector, get_data(element));
        }
    }
    return vector;
}

static Vector *vector_map(Vector *this, struct memory_functions *memory_functions) {
#ifdef __DEBUG__
    if (memory_functions->allocate_memory == NULL || memory_functions->free_memory == NULL ||
        memory_functions->map_allocate_memory == NULL) {
        fprintf(stderr, "You can't use vector map without a memory allocation / free function\n");
        return NULL;
    }
#endif
    Vector *vector = vector_init(memory_functions);
    for (size_t i = 0; i < this->length; i++) {
        VectorElement *new_element = vector->map_allocate_memory(this->get(this, i));
        vector_map_add(vector, new_element);
    }
    return vector;
}

static VectorElement *vector_reduce(Vector *this, void (*reducer)(VectorElement *, VectorElement *), void *init_value,
                                    struct memory_functions *memory_functions) {
    VectorElement *reduced_data = calloc(sizeof(struct VectorElement), 1);
    reduced_data->free_memory = memory_functions->free_memory;
    reduced_data->accumulator = memory_functions->allocate_memory(init_value);
    for (size_t i = 0; i < this->length; i++) {
        reducer(reduced_data, this->get(this, i));
    }
    return reduced_data;
}

static VectorElement *vector_find(Vector *this, bool (*predicate)(VectorElement *)) {
#ifdef __DEBUG__
    if (predicate == NULL) {
        fprintf(stderr, "You can't use vector find without a predicate function\n");
        return NULL;
    }
#endif
    void *found = NULL;
    for (size_t i = 0; i < this->length; i++) {
        VectorElement *element = this->get(this, i);
        if (predicate(element)) {
            found = element;
            break;
        }
    }
    return found;
}

Vector *vector_init(struct memory_functions *memory_functions) {
    Vector *vector = calloc(sizeof(struct Vector), 1);
    vector->allocate_memory = memory_functions->allocate_memory;
    vector->free_memory = memory_functions->free_memory;
    vector->map_allocate_memory = memory_functions->map_allocate_memory;
    vector->length = 0;
    vector->internal_size = VECTOR_INITIAL_SIZE;
    vector->elements = calloc(sizeof(struct VectorElement *), vector->internal_size);
    vector->add = vector_add;
    vector->remove = vector_remove;
    vector->get = vector_get;
    vector->filter = vector_filter;
    vector->map = vector_map;
    vector->reduce = vector_reduce;
    vector->find = vector_find;
    return vector;
}

Vector *vector_quit(Vector *vector) {
    if (vector) {
        if (vector->elements) {
            for (size_t i = 0; i < vector->length; i++) {
                VectorElement *element = vector->elements[i];
                if (element) {
                    element->free_memory(element);
                }
            }
            free(vector->elements);
            vector->elements = NULL;
        }
        vector->add = NULL;
        vector->get = NULL;
        vector->map = NULL;
        vector->find = NULL;
        vector->filter = NULL;
        vector->reduce = NULL;
        vector->remove = NULL;
        vector->allocate_memory = NULL;
        vector->free_memory = NULL;
        vector->map_allocate_memory = NULL;
        vector->internal_size = 0;
        vector->length = 0;
        free(vector);
        vector = NULL;
    }
    return vector;
}
