#ifdef __RUN_TESTS__
#include "minunit.h"
#endif

#include "vector.h"
#include <stdlib.h>
#include <string.h>

static const size_t VECTOR_INITIAL_SIZE = 1;

static void double_data_size(Vector* this)
{
    this->internal_size *= 2;
    this->data = realloc(this->data, this->internal_size * sizeof(void*));
}

static void vector_add(Vector* this, void* data)
{
    if (this->length == this->internal_size) {
        double_data_size(this);
    }
    this->data[this->length] = this->allocate_memory(data);
    this->length++;
}

static void vector_map_add(Vector* this, void* data)
{
    if (this->length == this->internal_size) {
        double_data_size(this);
    }
    this->data[this->length] = data;
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
        this->free_memory(this->data[position]);
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

static Vector* vector_filter(Vector* this, bool (*predicate)(void* data), struct memory_functions* memory_functions)
{
#ifdef __DEBUG__
    if (memory_functions->allocate_memory == NULL || memory_functions->free_memory == NULL) {
        fprintf(stderr, "You can't use vector filter without a memory allocation / free function\n");
        return NULL;
    }
#endif
    Vector* vector = vector_init(memory_functions);
    for (size_t i = 0; i < this->length; i++) {
        void* temp = this->get(this, i);
        if (predicate(temp)) {
            vector->add(vector, temp);
        }
    }
    return vector;
}

static Vector* vector_map(Vector* this, struct memory_functions* memory_functions)
{
#ifdef __DEBUG__
    if (memory_functions->allocate_memory == NULL || memory_functions->free_memory == NULL) {
        fprintf(stderr, "You can't use vector map without a memory allocation / free function\n");
        return NULL;
    }
#endif
    Vector* vector = vector_init(memory_functions);
    for (size_t i = 0; i < this->length; i++) {
        vector_map_add(vector, vector->allocate_memory(this->get(this, i)));
    }
    return vector;
}

static ReducedData* vector_reduce(Vector* this, void (*reducer)(void*, void*), void* init_value, struct memory_functions* memory_functions) {
    ReducedData* reduced_data = calloc(sizeof(struct ReducedData), 1);
    reduced_data->free_memory = memory_functions->free_memory;
    reduced_data->accumulator = memory_functions->allocate_memory(init_value);
    for (size_t i=0; i<this->length; i++) {
        reducer(reduced_data->accumulator, this->get(this, i));
    }
    return reduced_data;
}

static void* vector_find(Vector* this, bool (*predicate)(void* data))
{
#ifdef __DEBUG__
    if (predicate == NULL) {
        fprintf(stderr, "You can't use vector find without a predicate function\n");
        return NULL;
    }
#endif
    void* found = NULL;
    for (size_t i = 0; i < this->length; i++) {
        void* temp = this->get(this, i);
        if (predicate(temp)) {
            found = temp;
            break;
        }
    }
    return found;
}

Vector* vector_init(struct memory_functions* memory_functions)
{
    Vector* vector = calloc(sizeof(struct Vector), 1);
    vector->allocate_memory = memory_functions->allocate_memory;
    vector->free_memory = memory_functions->free_memory;
    vector->length = 0;
    vector->internal_size = VECTOR_INITIAL_SIZE;
    vector->data = calloc(sizeof(void*), vector->internal_size);
    vector->add = vector_add;
    vector->remove = vector_remove;
    vector->get = vector_get;
    vector->filter = vector_filter;
    vector->map = vector_map;
    vector->reduce = vector_reduce;
    vector->find = vector_find;
    return vector;
}

void vector_quit(Vector* vector)
{
    if (vector) {
        if (vector->data) {
            for (size_t i = 0; i < vector->internal_size; i++) {
                vector->free_memory(vector->data[i]);
            }
            free(vector->data);
            vector->data = NULL;
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
        vector->internal_size = 0;
        vector->length = 0;
        free(vector);
        vector = NULL;
    }
}

#ifdef __RUN_TESTS__

Vector* vector = NULL;
Vector* filtered_vector = NULL;
Vector* mapped_vector = NULL;

typedef struct TestData {
    size_t value;
    char* buffer;
} TestData;

typedef struct MappedTestData {
    char* mapped_buffer;
} MappedTestData;

static void* allocate_memory(void* data)
{
    TestData* output = calloc(sizeof(struct TestData), 1);
    if (output) {
        output->buffer = calloc(sizeof(char), strlen(data) + 1);
        strcpy(output->buffer, data);
        output->value = 1;
    }
    return output;
}

static void free_memory(void* data)
{
    TestData* input = data;
    if (input) {
        if (input->buffer) {
            free(input->buffer);
            input->buffer = NULL;
        }
        free(input);
        input = NULL;
    }
}

static int create_vector()
{
    struct memory_functions memory_functions = {
        allocate_memory, free_memory
    };
    vector = vector_init(&memory_functions);
    return vector != NULL && vector->length == 0;
}

static int adding_elements()
{
    vector->add(vector, "my test data 1");
    vector->add(vector, "test data 2");
    vector->add(vector, "my test data 3");
    vector->add(vector, "test data 4");
    vector->add(vector, "my test data 5");
    vector->add(vector, "test data 6");
    vector->add(vector, "my test data 7");
    vector->add(vector, "test data 8");
    vector->add(vector, "my test data 9");
    vector->add(vector, "test data 10");
    TestData* td9 = vector->get(vector, 9);
    TestData* td8 = vector->get(vector, 8);
    TestData* td7 = vector->get(vector, 7);
    TestData* td6 = vector->get(vector, 6);
    TestData* td5 = vector->get(vector, 5);
    TestData* td4 = vector->get(vector, 4);
    TestData* td3 = vector->get(vector, 3);
    TestData* td2 = vector->get(vector, 2);
    TestData* td1 = vector->get(vector, 1);
    TestData* td0 = vector->get(vector, 0);
    return vector->length == 10 && !strcmp(td0->buffer, "my test data 1") && !strcmp(td1->buffer, "test data 2")
        && !strcmp(td2->buffer, "my test data 3") && !strcmp(td3->buffer, "test data 4")
        && !strcmp(td4->buffer, "my test data 5") && !strcmp(td5->buffer, "test data 6")
        && !strcmp(td6->buffer, "my test data 7") && !strcmp(td7->buffer, "test data 8")
        && !strcmp(td8->buffer, "my test data 9") && !strcmp(td9->buffer, "test data 10");
}

static bool predicate(void* data)
{
    TestData* input = data;
    return input->buffer[0] == 'm';
}

static void* filter_allocate_memory(void* data)
{
    TestData* input = data;
    TestData* output = calloc(sizeof(struct TestData), 1);
    if (output) {
        output->buffer = calloc(sizeof(char), strlen(input->buffer) + 1);
        strcpy(output->buffer, input->buffer);
        output->value = 2;
    }
    return output;
}

static int filtering_elements()
{
    struct memory_functions memory_functions = {
        filter_allocate_memory, free_memory
    };
    filtered_vector = vector->filter(vector, predicate, &memory_functions);
    return filtered_vector->length == 5;
}

static void* map_allocate_memory(void* data)
{
    TestData* input = data;
    MappedTestData* output = calloc(sizeof(struct MappedTestData), 1);
    output->mapped_buffer = calloc(sizeof(char), strlen(input->buffer) + 8);
    sprintf(output->mapped_buffer, "mapped:%s", input->buffer);
    return output;
}

static void map_free_memory(void* data)
{
    MappedTestData* input = data;
    if (input) {
        if (input->mapped_buffer) {
            free(input->mapped_buffer);
            input->mapped_buffer = NULL;
        }
        free(input);
        input = NULL;
    }
}

static int mapping_elements()
{
    struct memory_functions memory_functions = {
        map_allocate_memory, map_free_memory
    };
    mapped_vector = vector->map(vector, &memory_functions);
    return mapped_vector->length == 10;
}

static void reducer(void* accumulator, void* data) {
    TestData* input = data;
    size_t accumulator_size = sizeof(char) * strlen(accumulator);
    size_t data_size = sizeof(char) * strlen(input->buffer);
    accumulator = realloc(accumulator, accumulator_size + data_size + 1);
    accumulator = strcat(accumulator, input->buffer);
}

static void* reduce_allocate_memory(void* data)
{
    char* output = calloc(sizeof(char), strlen(data) + 1);
    strcpy(output, data);
    return output;
}

static void reduce_free_memory(void* data)
{
    ReducedData* reduced = data;
    if (reduced) {
        if (reduced->accumulator) {
            free(reduced->accumulator);
            reduced->accumulator = NULL;
        }
        reduced->free_memory = NULL;
        free(reduced);
        reduced = NULL;
    }
}

static int reduce_strings() {
    struct memory_functions memory_functions = {
        .allocate_memory = reduce_allocate_memory,
        .free_memory = reduce_free_memory
    };
    ReducedData* reduced_data = filtered_vector->reduce(filtered_vector, reducer, "", &memory_functions);
    bool result = !strcmp(reduced_data->accumulator, "my test data 1my test data 3my test data 5my test data 7my test data 9");
    reduced_data->free_memory(reduced_data);
    return result;
}

static bool find_test_data_3(void* data) {
    TestData* input = data;
    return !strcmp(input->buffer, "my test data 3");
}

static bool find_non_existing_data(void* data) {
    TestData* input = data;
    return !strcmp(input->buffer, "no data found");
}

static int find_element() {
    TestData* test_data = vector->find(vector, find_test_data_3);
    return !strcmp(test_data->buffer, "my test data 3");
}

static int element_not_found() {
    TestData* test_data = vector->find(vector, find_non_existing_data);
    return test_data == NULL;
}

static int destroy_vector()
{
    vector_quit(mapped_vector);
    vector_quit(filtered_vector);
    vector_quit(vector);
    return vector == NULL && mapped_vector == NULL && filtered_vector == NULL;
}

int main(void)
{
    run_test("create_vector", create_vector);
    run_test("adding_elements", adding_elements);
    run_test("filtering_elements", filtering_elements);
    run_test("mapping_elements", mapping_elements);
    run_test("reduce_strings", reduce_strings);
    run_test("find_element", find_element);
    run_test("element_not_found", element_not_found);
    run_test("destroy_vector", destroy_vector);
    show_tests_result;
}

#endif
