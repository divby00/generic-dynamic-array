#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>
#include "vector.h"

#include "minunit.h"

Vector *vector = NULL;
Vector *filtered_vector = NULL;
Vector *mapped_vector = NULL;
Vector *big_vector = NULL;

typedef struct TestData {
    char *buffer;
} TestData;

typedef struct MappedTestData {
    char *mapped_buffer;
} MappedTestData;

static void show_vector(Vector *vector) {
    for (size_t i = 0; i < vector->length; i++) {
        VectorElement *element = vector->get(vector, i);
        TestData *test_data = element->data;
        fprintf(stdout, "\nvec: %s", test_data->buffer);
    }
}

static void show_mapped_vector(Vector *vector) {
    for (size_t i = 0; i < vector->length; i++) {
        VectorElement *element = vector->get(vector, i);
        MappedTestData *mapped_data = element->data;
        fprintf(stdout, "\nvec: %s", mapped_data->mapped_buffer);
    }
}

static VectorElement *allocate_memory(void *data) {
    VectorElement *element = calloc(sizeof(struct VectorElement), 1);
    if (element) {
        TestData *test_data = calloc(sizeof(struct TestData), 1);
        if (test_data) {
            test_data->buffer = calloc(sizeof(char), strlen(data) + 1);
            strcpy(test_data->buffer, data);
        }
        element->data = test_data;
    }
    return element;
}

static void free_memory(VectorElement *element) {
    if (element) {
        TestData *data = element->data;
        if (data) {
            if (data->buffer) {
                free(data->buffer);
                data->buffer = NULL;
            }
            free(data);
            data = NULL;
        }
        element->free_memory = NULL;
        element->accumulator = NULL;
        free(element);
        element = NULL;
    }
}

static int create_vector() {
    struct memory_functions memory_functions = {
        .allocate_memory = allocate_memory,
        .free_memory = free_memory,
        .map_allocate_memory = NULL
    };
    vector = vector_init(&memory_functions);
    return vector != NULL && vector->length == 0;
}

static int adding_elements() {
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

    TestData *td0 = (TestData *) (vector->get(vector, 0))->data;
    TestData *td1 = (TestData *) (vector->get(vector, 1))->data;
    TestData *td2 = (TestData *) (vector->get(vector, 2))->data;
    TestData *td3 = (TestData *) (vector->get(vector, 3))->data;
    TestData *td4 = (TestData *) (vector->get(vector, 4))->data;
    TestData *td5 = (TestData *) (vector->get(vector, 5))->data;
    TestData *td6 = (TestData *) (vector->get(vector, 6))->data;
    TestData *td7 = (TestData *) (vector->get(vector, 7))->data;
    TestData *td8 = (TestData *) (vector->get(vector, 8))->data;
    TestData *td9 = (TestData *) (vector->get(vector, 9))->data;

    return vector->length == 10
           && !strcmp(td0->buffer, "my test data 1") && !strcmp(td1->buffer, "test data 2")
           && !strcmp(td2->buffer, "my test data 3") && !strcmp(td3->buffer, "test data 4")
           && !strcmp(td4->buffer, "my test data 5") && !strcmp(td5->buffer, "test data 6")
           && !strcmp(td6->buffer, "my test data 7") && !strcmp(td7->buffer, "test data 8")
           && !strcmp(td8->buffer, "my test data 9") && !strcmp(td9->buffer, "test data 10");
}

static int removing_elements() {
    vector->remove(vector, 0);
    vector->remove(vector, 0);
    vector->remove(vector, 0);
    vector->remove(vector, 0);
    vector->remove(vector, 0);

    TestData *td0 = (TestData *) (vector->get(vector, 0))->data;
    TestData *td1 = (TestData *) (vector->get(vector, 1))->data;
    TestData *td2 = (TestData *) (vector->get(vector, 2))->data;
    TestData *td3 = (TestData *) (vector->get(vector, 3))->data;
    TestData *td4 = (TestData *) (vector->get(vector, 4))->data;

    return vector->length == 5
           && !strcmp(td0->buffer, "test data 6") && !strcmp(td1->buffer, "my test data 7")
           && !strcmp(td2->buffer, "test data 8") && !strcmp(td3->buffer, "my test data 9")
           && !strcmp(td4->buffer, "test data 10");
}

static bool predicate(VectorElement *element) {
    TestData *test_data = element->data;
    return test_data->buffer[0] == 'm';
}

static void *get_vector_data(VectorElement *element) {
    TestData *test_data = element->data;
    return test_data->buffer;
}

static int filtering_elements() {
    filtered_vector = vector->filter(vector, predicate, get_vector_data);
    TestData *td0 = (TestData *) (filtered_vector->get(filtered_vector, 0))->data;
    TestData *td1 = (TestData *) (filtered_vector->get(filtered_vector, 1))->data;
    TestData *td2 = (TestData *) (filtered_vector->get(filtered_vector, 2))->data;
    TestData *td3 = (TestData *) (filtered_vector->get(filtered_vector, 3))->data;
    TestData *td4 = (TestData *) (filtered_vector->get(filtered_vector, 4))->data;

    return filtered_vector->length == 5
           && !strcmp(td0->buffer, "my test data 1") && !strcmp(td1->buffer, "my test data 3")
           && !strcmp(td2->buffer, "my test data 5") && !strcmp(td3->buffer, "my test data 7")
           && !strcmp(td4->buffer, "my test data 9");
}

static int adding_elements_in_filtered_vector() {
    filtered_vector->add(filtered_vector, "additional test data 01");
    filtered_vector->add(filtered_vector, "additional test data 02");

    TestData *td0 = (TestData *) (filtered_vector->get(filtered_vector, 0))->data;
    TestData *td1 = (TestData *) (filtered_vector->get(filtered_vector, 1))->data;
    TestData *td2 = (TestData *) (filtered_vector->get(filtered_vector, 2))->data;
    TestData *td3 = (TestData *) (filtered_vector->get(filtered_vector, 3))->data;
    TestData *td4 = (TestData *) (filtered_vector->get(filtered_vector, 4))->data;

    TestData *additional01 = (TestData *) (filtered_vector->get(filtered_vector, 5))->data;
    TestData *additional02 = (TestData *) (filtered_vector->get(filtered_vector, 6))->data;

    return filtered_vector->length == 7
           && !strcmp(td0->buffer, "my test data 1") && !strcmp(td1->buffer, "my test data 3")
           && !strcmp(td0->buffer, "my test data 1") && !strcmp(td1->buffer, "my test data 3")
           && !strcmp(td2->buffer, "my test data 5") && !strcmp(td3->buffer, "my test data 7")
           && !strcmp(td4->buffer, "my test data 9") && !strcmp(additional01->buffer, "additional test data 01")
           && !strcmp(additional02->buffer, "additional test data 02");
}

static int removing_elements_in_filtered_vector() {
    filtered_vector->remove(filtered_vector, 6);
    filtered_vector->remove(filtered_vector, 5);
    filtered_vector->remove(filtered_vector, 4);
    filtered_vector->remove(filtered_vector, 3);
    filtered_vector->remove(filtered_vector, 2);

    TestData *td0 = (TestData *) (filtered_vector->get(filtered_vector, 0))->data;
    TestData *td1 = (TestData *) (filtered_vector->get(filtered_vector, 1))->data;

    return filtered_vector->length == 2
           && !strcmp(td0->buffer, "my test data 1") && !strcmp(td1->buffer, "my test data 3");
}

static VectorElement *map_allocate_memory(void *element) {
    VectorElement *old_element = element;
    TestData *test_data = old_element->data;
    VectorElement *new_element = calloc(sizeof(struct VectorElement), 1);
    MappedTestData *mapped_data = calloc(sizeof(struct MappedTestData), 1);
    mapped_data->mapped_buffer = calloc(sizeof(char), strlen(test_data->buffer) + 8);
    sprintf(mapped_data->mapped_buffer, "mapped:%s", test_data->buffer);
    new_element->data = mapped_data;
    return new_element;
}

static void map_free_memory(VectorElement *element) {
    if (element) {
        MappedTestData *mapped_data = element->data;
        if (mapped_data) {
            if (mapped_data->mapped_buffer) {
                free(mapped_data->mapped_buffer);
                mapped_data->mapped_buffer = NULL;
            }
            free(mapped_data);
            element->data = mapped_data = NULL;
        }
        element->free_memory = NULL;
        element->accumulator = NULL;
        free(element);
        element = NULL;
    }
}

static int mapping_elements() {
    struct memory_functions memory_functions = {
        .allocate_memory = allocate_memory,
        .free_memory = map_free_memory,
        .map_allocate_memory = map_allocate_memory
    };
    mapped_vector = vector->map(vector, &memory_functions);
    return mapped_vector->length == 5;
}

static int adding_elements_in_mapped_vector() {
    mapped_vector->add(mapped_vector, "moremappeddata0");
    mapped_vector->add(mapped_vector, "moremappeddata1");
    mapped_vector->add(mapped_vector, "moremappeddata2");

    MappedTestData *td0 = (MappedTestData *) (mapped_vector->get(mapped_vector, 0))->data;
    MappedTestData *td1 = (MappedTestData *) (mapped_vector->get(mapped_vector, 1))->data;
    MappedTestData *td2 = (MappedTestData *) (mapped_vector->get(mapped_vector, 2))->data;
    MappedTestData *td3 = (MappedTestData *) (mapped_vector->get(mapped_vector, 3))->data;
    MappedTestData *td4 = (MappedTestData *) (mapped_vector->get(mapped_vector, 4))->data;
    MappedTestData *td5 = (MappedTestData *) (mapped_vector->get(mapped_vector, 5))->data;
    MappedTestData *td6 = (MappedTestData *) (mapped_vector->get(mapped_vector, 6))->data;
    MappedTestData *td7 = (MappedTestData *) (mapped_vector->get(mapped_vector, 7))->data;

    return mapped_vector->length == 8
        && !strcmp(td0->mapped_buffer, "mapped:test data 6") && !strcmp(td1->mapped_buffer, "mapped:my test data 7")
        && !strcmp(td2->mapped_buffer, "mapped:test data 8") && !strcmp(td3->mapped_buffer, "mapped:my test data 9")
        && !strcmp(td4->mapped_buffer, "mapped:test data 10") && !strcmp(td5->mapped_buffer, "moremappeddata0")
        && !strcmp(td6->mapped_buffer, "moremappeddata1") && !strcmp(td7->mapped_buffer, "moremappeddata2");
}

static void reducer(void *accumulator, void *data) {
    TestData *input = data;
    size_t accumulator_size = sizeof(char) * strlen(accumulator);
    size_t data_size = sizeof(char) * strlen(input->buffer);
    accumulator = realloc(accumulator, accumulator_size + data_size + 1);
    accumulator = strcat(accumulator, input->buffer);
}

static void *reduce_allocate_memory(void *data) {
    char *output = calloc(sizeof(char), strlen(data) + 1);
    strcpy(output, data);
    return output;
}

static void reduce_free_memory(void *data) {
    VectorElement *reduced = data;
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
        .free_memory = reduce_free_memory,
        .map_allocate_memory = NULL
    };
    VectorElement *reduced_data = filtered_vector->reduce(filtered_vector, reducer, "", &memory_functions);
    bool result = !strcmp(reduced_data->accumulator,
                          "my test data 1my test data 3my test data 5my test data 7my test data 9");
    reduced_data->free_memory(reduced_data);
    return result;
}

static bool find_test_data_7(VectorElement *element) {
    TestData *test_data = element->data;
    return !strcmp(test_data->buffer, "my test data 7");
}

static bool find_non_existing_data(VectorElement *element) {
    TestData *test_data = element->data;
    return !strcmp(test_data->buffer, "no data found");
}

static int find_element() {
    VectorElement* element = vector->find(vector, find_test_data_7);
    TestData* test_data = element->data;
    return !strcmp(test_data->buffer, "my test data 7");
}

static int element_not_found() {
    VectorElement* element = vector->find(vector, find_non_existing_data);
    return element == NULL;
}

static int destroy_vectors() {
    mapped_vector = vector_quit(mapped_vector);
    filtered_vector = vector_quit(filtered_vector);
    vector = vector_quit(vector);
    big_vector = vector_quit(big_vector);
    return vector == NULL && mapped_vector == NULL && filtered_vector == NULL && big_vector == NULL;
}

VectorElement* allocate_number(void* number) {
    VectorElement* element = calloc(sizeof(struct VectorElement), 1);
    element->data = calloc(sizeof(size_t), 1);
    *((size_t*)element->data) = *((size_t*)number);
    return element;
}

void free_number(VectorElement* element) {
    if (element) {
        if (element->data) {
            free(element->data);
            element->data = NULL;
        }
        element->free_memory = NULL;
        element->accumulator = NULL;
        free(element);
        element = NULL;
    }
}

static uint64_t get_posix_clock_time ()
{
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
        return (uint64_t)(ts.tv_sec * 1000000 + ts.tv_nsec / 1000);
    } else {
        return 0;
    }
}

static int inserting_a_million_records() {
    struct memory_functions memory_functions = {
        .allocate_memory = allocate_number,
        .free_memory = free_number,
        .map_allocate_memory = NULL
    };
    big_vector = vector_init(&memory_functions);
    uint64_t prev_time_value, time_value;
    prev_time_value = get_posix_clock_time();
    for(size_t i=0; i<1000000; i++) {
        big_vector->add(big_vector, &i);
    }
    time_value = get_posix_clock_time();
    float time_diff = (float)(time_value - prev_time_value) / 1000000;
    fprintf(stdout, "Inserted a million records in %lf seconds\n", time_diff);
    return big_vector->length == 1000000;
}

int main(void) {
    run_test("creating vector", create_vector);
    run_test("adding elements", adding_elements);
    run_test("filtering elements", filtering_elements);
    run_test("adding elements in filtered vector", adding_elements_in_filtered_vector);
    run_test("removing elements in filtered vector", removing_elements_in_filtered_vector);
    run_test("removing elements", removing_elements);
    run_test("mapping_elements", mapping_elements);
    run_test("adding elements in mapped vector", adding_elements_in_mapped_vector);
    run_test("inserting a million records", inserting_a_million_records);
    run_test("find_element", find_element);
    run_test("element_not_found", element_not_found);
    /*
    run_test("reduce_strings", reduce_strings);
    */
    run_test("destroying vectors", destroy_vectors);
    show_tests_result;
    return 0;
}
