#include <stdlib.h>
#include <string.h>
#include "array.h"

static const size_t ARRAY_INITIAL_SIZE = 2;

static void double_data_size(Array *this) {
    this->internal_size += this->internal_size / 2;
    this->elements = realloc(this->elements, this->internal_size * sizeof(struct ArrayElement *));
}

static void array_add(Array *this, void *data) {
    if (this->length == this->internal_size) {
        double_data_size(this);
    }
    ArrayElement *element = this->allocate_memory(data);
    element->free_memory = this->free_memory;
    this->elements[this->length] = element;
    this->length++;
}

static void array_map_add(Array *this, ArrayElement *element) {
    if (this->length == this->internal_size) {
        double_data_size(this);
    }
    element->free_memory = this->free_memory;
    this->elements[this->length] = element;
    this->length++;
}

static void array_remove(Array *this, size_t position) {
#ifdef __DEBUG__
    if (position < this->length && position >= 0) {
#endif
        this->internal_size--;
        this->length--;
        size_t first_offset = position * sizeof(struct ArrayElement *);
        size_t second_offset = (this->internal_size - position) * sizeof(struct ArrayElement *);
        ArrayElement **data = calloc(sizeof(struct ArrayElement *), this->internal_size);
        memcpy(data, this->elements, first_offset);
        memcpy(data + position, this->elements + position + 1, second_offset);
        ArrayElement *element = this->elements[position];
        element->free_memory(element);
        free(this->elements);
        this->elements = data;
#ifdef __DEBUG__
    } else {
        fprintf(stderr, "Out of bounds error! Trying to remove array element at position %ld\n", position);
    }
#endif
}

static ArrayElement *array_get(Array *this, size_t position) {
#ifdef __DEBUG__
    if (position < this->length && position >= 0) {
#endif
        return this->elements[position];
#ifdef __DEBUG__
    } else {
        fprintf(stderr, "Out of bounds error! trying to get array element at position %ld\n", position);
        return NULL;
    }
#endif
}

static Array *array_filter(Array *this, bool (*predicate)(ArrayElement *), void *(*get_data)(ArrayElement *)) {
#ifdef __DEBUG__
    if (this->allocate_memory == NULL || this->free_memory == NULL) {
        fprintf(stderr, "You can't use array filter without a memory allocation / free function\n");
        return NULL;
    }
#endif
    struct memory_functions memory_functions = {
            .allocate_memory = this->allocate_memory,
            .free_memory = this->free_memory,
            .map_allocate_memory = this->map_allocate_memory
    };
    Array *array = array_init(&memory_functions);
    for (size_t i = 0; i < this->length; i++) {
        ArrayElement *element = this->get(this, i);
        if (predicate(element)) {
            array->add(array, get_data(element));
        }
    }
    return array;
}

static Array *array_map(Array *this, struct memory_functions *memory_functions) {
#ifdef __DEBUG__
    if (memory_functions->allocate_memory == NULL || memory_functions->free_memory == NULL ||
        memory_functions->map_allocate_memory == NULL) {
        fprintf(stderr, "You can't use array map without a memory allocation / free function\n");
        return NULL;
    }
#endif
    Array *array = array_init(memory_functions);
    for (size_t i = 0; i < this->length; i++) {
        ArrayElement *new_element = array->map_allocate_memory(this->get(this, i));
        array_map_add(array, new_element);
    }
    return array;
}

static ArrayElement *array_reduce(Array *this, void (*reducer)(ArrayElement *, ArrayElement *), void *init_value,
                                  struct memory_functions *memory_functions) {
    ArrayElement *reduced_data = calloc(sizeof(struct ArrayElement), 1);
    reduced_data->free_memory = memory_functions->free_memory;
    reduced_data->accumulator = memory_functions->allocate_memory(init_value);
    for (size_t i = 0; i < this->length; i++) {
        reducer(reduced_data, this->get(this, i));
    }
    return reduced_data;
}

static ArrayElement *array_find(Array *this, bool (*predicate)(ArrayElement *)) {
#ifdef __DEBUG__
    if (predicate == NULL) {
        fprintf(stderr, "You can't use array find without a predicate function\n");
        return NULL;
    }
#endif
    void *found = NULL;
    for (size_t i = 0; i < this->length; i++) {
        ArrayElement *element = this->get(this, i);
        if (predicate(element)) {
            found = element;
            break;
        }
    }
    return found;
}

Array *array_init(struct memory_functions *memory_functions) {
    Array *array = calloc(sizeof(struct Array), 1);
    array->allocate_memory = memory_functions->allocate_memory;
    array->free_memory = memory_functions->free_memory;
    array->map_allocate_memory = memory_functions->map_allocate_memory;
    array->length = 0;
    array->internal_size = ARRAY_INITIAL_SIZE;
    array->elements = calloc(sizeof(struct ArrayElement *), array->internal_size);
    array->add = array_add;
    array->remove = array_remove;
    array->get = array_get;
    array->filter = array_filter;
    array->map = array_map;
    array->reduce = array_reduce;
    array->find = array_find;
    return array;
}

Array *array_quit(Array *array) {
    if (array) {
        if (array->elements) {
            for (size_t i = 0; i < array->length; i++) {
                ArrayElement *element = array->elements[i];
                if (element) {
                    element->free_memory(element);
                }
            }
            free(array->elements);
            array->elements = NULL;
        }
        array->add = NULL;
        array->get = NULL;
        array->map = NULL;
        array->find = NULL;
        array->filter = NULL;
        array->reduce = NULL;
        array->remove = NULL;
        array->allocate_memory = NULL;
        array->free_memory = NULL;
        array->map_allocate_memory = NULL;
        array->internal_size = 0;
        array->length = 0;
        free(array);
        array = NULL;
    }
    return array;
}
