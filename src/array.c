/**
 * Copyright (c) 2019 divby0
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "array.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static const size_t ARRAY_INITIAL_SIZE = 2;

static void increase_array_size(Array* this)
{
    assert(this);
    this->internal_size += this->internal_size / 2;
    this->elements = realloc(this->elements, this->internal_size * sizeof(struct ArrayElement*));
}

static void array_add(Array* this, void* data)
{
    assert(this && data);
    if (this->length == this->internal_size) {
        increase_array_size(this);
    }
    ArrayElement* element = this->allocate_memory(data);
    element->free_memory = this->free_memory;
    this->elements[this->length] = element;
    this->length++;
}

static void array_map_add(Array* this, ArrayElement* element)
{
    assert(this && element);
    if (this->length == this->internal_size) {
        increase_array_size(this);
    }
    element->free_memory = this->free_memory;
    this->elements[this->length] = element;
    this->length++;
}

static void array_remove(Array* this, size_t position)
{
    assert(position < this->length && position >= 0);
    this->internal_size--;
    this->length--;
    size_t first_offset = position * sizeof(struct ArrayElement*);
    size_t second_offset = (this->internal_size - position) * sizeof(struct ArrayElement*);
    ArrayElement** data = calloc(sizeof(struct ArrayElement*), this->internal_size);
    memcpy(data, this->elements, first_offset);
    memcpy(data + position, this->elements + position + 1, second_offset);
    ArrayElement* element = this->elements[position];
    element->free_memory(element);
    free(this->elements);
    this->elements = data;
}

static ArrayElement* array_get(Array* this, size_t position)
{
    assert(this && (position < this->length && position >= 0));
    return this->elements[position];
}

static Array* array_filter(Array* this, bool (*predicate)(ArrayElement*), void* (*get_data)(ArrayElement*))
{
    assert(this && this->allocate_memory && this->free_memory && predicate && get_data);
    struct memory_functions memory_functions = {
        .allocate_memory = this->allocate_memory,
        .free_memory = this->free_memory,
        .map_allocate_memory = this->map_allocate_memory
    };
    Array* array = array_init(&memory_functions);
    for (size_t i = 0; i < this->length; i++) {
        ArrayElement* element = this->get(this, i);
        if (predicate(element)) {
            array->add(array, get_data(element));
        }
    }
    return array;
}

static Array* array_map(Array* this, struct memory_functions* memory_functions)
{
    assert(this && memory_functions->allocate_memory && memory_functions->free_memory && memory_functions->map_allocate_memory);
    Array* array = array_init(memory_functions);
    for (size_t i = 0; i < this->length; i++) {
        ArrayElement* new_element = array->map_allocate_memory(this->get(this, i));
        array_map_add(array, new_element);
    }
    return array;
}

static ArrayElement* array_reduce(Array* this, void (*reducer)(ArrayElement*, ArrayElement*), void* init_value,
    struct memory_functions* memory_functions)
{
    assert(this && reducer && memory_functions->free_memory && memory_functions->allocate_memory);
    ArrayElement* reduced_data = calloc(sizeof(struct ArrayElement), 1);
    reduced_data->free_memory = memory_functions->free_memory;
    reduced_data->accumulator = memory_functions->allocate_memory(init_value);
    for (size_t i = 0; i < this->length; i++) {
        reducer(reduced_data, this->get(this, i));
    }
    return reduced_data;
}

static ArrayElement* array_find(Array* this, bool (*predicate)(ArrayElement*))
{
    assert(this && predicate);
    void* found = NULL;
    for (size_t i = 0; i < this->length; i++) {
        ArrayElement* element = this->get(this, i);
        if (predicate(element)) {
            found = element;
            break;
        }
    }
    return found;
}

Array* array_init(struct memory_functions* memory_functions)
{
    assert(memory_functions && memory_functions->allocate_memory && memory_functions->free_memory);
    Array* array = calloc(sizeof(struct Array), 1);
    array->allocate_memory = memory_functions->allocate_memory;
    array->free_memory = memory_functions->free_memory;
    array->map_allocate_memory = memory_functions->map_allocate_memory;
    array->length = 0;
    array->internal_size = ARRAY_INITIAL_SIZE;
    array->elements = calloc(sizeof(struct ArrayElement*), array->internal_size);
    array->add = array_add;
    array->remove = array_remove;
    array->get = array_get;
    array->filter = array_filter;
    array->map = array_map;
    array->reduce = array_reduce;
    array->find = array_find;
    return array;
}

Array* array_quit(Array* array)
{
    assert(array);
    if (array->elements) {
        for (size_t i = 0; i < array->length; i++) {
            ArrayElement* element = array->elements[i];
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
    array->allocate_memory = array->map_allocate_memory = NULL;
    array->free_memory = NULL;
    array->internal_size = array->length = 0;
    free(array);
    array = NULL;
    return array;
}
