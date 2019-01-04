#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>
#include "array.h"

#include "minunit.h"

Array *array = NULL;
Array *filtered_array = NULL;
Array *mapped_array = NULL;
Array *big_array = NULL;

typedef struct TestData {
    char *buffer;
} TestData;

typedef struct MappedTestData {
    char *mapped_buffer;
} MappedTestData;

static void show_array(Array *array) {
    for (size_t i = 0; i < array->length; i++) {
        ArrayElement *element = array->get(array, i);
        TestData *test_data = element->data;
        fprintf(stdout, "\nvec: %s", test_data->buffer);
    }
}

static void show_mapped_array(Array *array) {
    for (size_t i = 0; i < array->length; i++) {
        ArrayElement *element = array->get(array, i);
        MappedTestData *mapped_data = element->data;
        fprintf(stdout, "\nvec: %s", mapped_data->mapped_buffer);
    }
}

static ArrayElement *allocate_memory(void *data) {
    ArrayElement *element = calloc(sizeof(struct ArrayElement), 1);
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

static void free_memory(ArrayElement *element) {
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

static int create_array() {
    struct memory_functions memory_functions = {
            .allocate_memory = allocate_memory,
            .free_memory = free_memory,
            .map_allocate_memory = NULL
    };
    array = array_init(&memory_functions);
    return array != NULL && array->length == 0;
}

static int adding_elements() {
    array->add(array, "my test data 1");
    array->add(array, "test data 2");
    array->add(array, "my test data 3");
    array->add(array, "test data 4");
    array->add(array, "my test data 5");
    array->add(array, "test data 6");
    array->add(array, "my test data 7");
    array->add(array, "test data 8");
    array->add(array, "my test data 9");
    array->add(array, "test data 10");

    TestData *td0 = (TestData *) (array->get(array, 0))->data;
    TestData *td1 = (TestData *) (array->get(array, 1))->data;
    TestData *td2 = (TestData *) (array->get(array, 2))->data;
    TestData *td3 = (TestData *) (array->get(array, 3))->data;
    TestData *td4 = (TestData *) (array->get(array, 4))->data;
    TestData *td5 = (TestData *) (array->get(array, 5))->data;
    TestData *td6 = (TestData *) (array->get(array, 6))->data;
    TestData *td7 = (TestData *) (array->get(array, 7))->data;
    TestData *td8 = (TestData *) (array->get(array, 8))->data;
    TestData *td9 = (TestData *) (array->get(array, 9))->data;

    return array->length == 10
           && !strcmp(td0->buffer, "my test data 1") && !strcmp(td1->buffer, "test data 2")
           && !strcmp(td2->buffer, "my test data 3") && !strcmp(td3->buffer, "test data 4")
           && !strcmp(td4->buffer, "my test data 5") && !strcmp(td5->buffer, "test data 6")
           && !strcmp(td6->buffer, "my test data 7") && !strcmp(td7->buffer, "test data 8")
           && !strcmp(td8->buffer, "my test data 9") && !strcmp(td9->buffer, "test data 10");
}

static int removing_elements() {
    array->remove(array, 0);
    array->remove(array, 0);
    array->remove(array, 0);
    array->remove(array, 0);
    array->remove(array, 0);

    TestData *td0 = (TestData *) (array->get(array, 0))->data;
    TestData *td1 = (TestData *) (array->get(array, 1))->data;
    TestData *td2 = (TestData *) (array->get(array, 2))->data;
    TestData *td3 = (TestData *) (array->get(array, 3))->data;
    TestData *td4 = (TestData *) (array->get(array, 4))->data;

    return array->length == 5
           && !strcmp(td0->buffer, "test data 6") && !strcmp(td1->buffer, "my test data 7")
           && !strcmp(td2->buffer, "test data 8") && !strcmp(td3->buffer, "my test data 9")
           && !strcmp(td4->buffer, "test data 10");
}

static bool predicate(ArrayElement *element) {
    TestData *test_data = element->data;
    return test_data->buffer[0] == 'm';
}

static void *get_array_data(ArrayElement *element) {
    TestData *test_data = element->data;
    return test_data->buffer;
}

static int filtering_elements() {
    filtered_array = array->filter(array, predicate, get_array_data);
    TestData *td0 = (TestData *) (filtered_array->get(filtered_array, 0))->data;
    TestData *td1 = (TestData *) (filtered_array->get(filtered_array, 1))->data;
    TestData *td2 = (TestData *) (filtered_array->get(filtered_array, 2))->data;
    TestData *td3 = (TestData *) (filtered_array->get(filtered_array, 3))->data;
    TestData *td4 = (TestData *) (filtered_array->get(filtered_array, 4))->data;

    return filtered_array->length == 5
           && !strcmp(td0->buffer, "my test data 1") && !strcmp(td1->buffer, "my test data 3")
           && !strcmp(td2->buffer, "my test data 5") && !strcmp(td3->buffer, "my test data 7")
           && !strcmp(td4->buffer, "my test data 9");
}

static int adding_elements_in_filtered_array() {
    filtered_array->add(filtered_array, "additional test data 01");
    filtered_array->add(filtered_array, "additional test data 02");

    TestData *td0 = (TestData *) (filtered_array->get(filtered_array, 0))->data;
    TestData *td1 = (TestData *) (filtered_array->get(filtered_array, 1))->data;
    TestData *td2 = (TestData *) (filtered_array->get(filtered_array, 2))->data;
    TestData *td3 = (TestData *) (filtered_array->get(filtered_array, 3))->data;
    TestData *td4 = (TestData *) (filtered_array->get(filtered_array, 4))->data;

    TestData *additional01 = (TestData *) (filtered_array->get(filtered_array, 5))->data;
    TestData *additional02 = (TestData *) (filtered_array->get(filtered_array, 6))->data;

    return filtered_array->length == 7
           && !strcmp(td0->buffer, "my test data 1") && !strcmp(td1->buffer, "my test data 3")
           && !strcmp(td0->buffer, "my test data 1") && !strcmp(td1->buffer, "my test data 3")
           && !strcmp(td2->buffer, "my test data 5") && !strcmp(td3->buffer, "my test data 7")
           && !strcmp(td4->buffer, "my test data 9") && !strcmp(additional01->buffer, "additional test data 01")
           && !strcmp(additional02->buffer, "additional test data 02");
}

static int removing_elements_in_filtered_array() {
    filtered_array->remove(filtered_array, 6);
    filtered_array->remove(filtered_array, 5);
    filtered_array->remove(filtered_array, 4);
    filtered_array->remove(filtered_array, 3);
    filtered_array->remove(filtered_array, 2);

    TestData *td0 = (TestData *) (filtered_array->get(filtered_array, 0))->data;
    TestData *td1 = (TestData *) (filtered_array->get(filtered_array, 1))->data;

    return filtered_array->length == 2
           && !strcmp(td0->buffer, "my test data 1") && !strcmp(td1->buffer, "my test data 3");
}

static ArrayElement *map_allocate_memory(void *element) {
    ArrayElement *old_element = element;
    TestData *test_data = old_element->data;
    ArrayElement *new_element = calloc(sizeof(struct ArrayElement), 1);
    MappedTestData *mapped_data = calloc(sizeof(struct MappedTestData), 1);
    mapped_data->mapped_buffer = calloc(sizeof(char), strlen(test_data->buffer) + 8);
    sprintf(mapped_data->mapped_buffer, "mapped:%s", test_data->buffer);
    new_element->data = mapped_data;
    return new_element;
}

static void map_free_memory(ArrayElement *element) {
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
    mapped_array = array->map(array, &memory_functions);
    return mapped_array->length == 5;
}

static int adding_elements_in_mapped_array() {
    mapped_array->add(mapped_array, "moremappeddata0");
    mapped_array->add(mapped_array, "moremappeddata1");
    mapped_array->add(mapped_array, "moremappeddata2");

    MappedTestData *td0 = (MappedTestData *) (mapped_array->get(mapped_array, 0))->data;
    MappedTestData *td1 = (MappedTestData *) (mapped_array->get(mapped_array, 1))->data;
    MappedTestData *td2 = (MappedTestData *) (mapped_array->get(mapped_array, 2))->data;
    MappedTestData *td3 = (MappedTestData *) (mapped_array->get(mapped_array, 3))->data;
    MappedTestData *td4 = (MappedTestData *) (mapped_array->get(mapped_array, 4))->data;
    MappedTestData *td5 = (MappedTestData *) (mapped_array->get(mapped_array, 5))->data;
    MappedTestData *td6 = (MappedTestData *) (mapped_array->get(mapped_array, 6))->data;
    MappedTestData *td7 = (MappedTestData *) (mapped_array->get(mapped_array, 7))->data;

    return mapped_array->length == 8
           && !strcmp(td0->mapped_buffer, "mapped:test data 6") && !strcmp(td1->mapped_buffer, "mapped:my test data 7")
           && !strcmp(td2->mapped_buffer, "mapped:test data 8") && !strcmp(td3->mapped_buffer, "mapped:my test data 9")
           && !strcmp(td4->mapped_buffer, "mapped:test data 10") && !strcmp(td5->mapped_buffer, "moremappeddata0")
           && !strcmp(td6->mapped_buffer, "moremappeddata1") && !strcmp(td7->mapped_buffer, "moremappeddata2");
}

static int removing_elements_in_mapped_array() {
    mapped_array->remove(mapped_array, mapped_array->length - 1);
    mapped_array->remove(mapped_array, mapped_array->length - 1);
    mapped_array->remove(mapped_array, mapped_array->length - 1);
    mapped_array->remove(mapped_array, mapped_array->length - 1);
    mapped_array->remove(mapped_array, mapped_array->length - 1);
    mapped_array->remove(mapped_array, mapped_array->length - 1);

    MappedTestData *td0 = (MappedTestData *) (mapped_array->get(mapped_array, 0))->data;
    MappedTestData *td1 = (MappedTestData *) (mapped_array->get(mapped_array, 1))->data;

    return mapped_array->length == 2
           && !strcmp(td0->mapped_buffer, "mapped:test data 6") && !strcmp(td1->mapped_buffer, "mapped:my test data 7");
}

/**
 * Accumulator contains the computed data, element is the array element to get the data from.
 */
static void reducer(ArrayElement *acc_element, ArrayElement *element) {
    size_t accumulator_size = sizeof(char) * strlen(acc_element->accumulator);
    TestData *test_data = element->data;
    size_t data_size = sizeof(char) * strlen(test_data->buffer);
    acc_element->accumulator = realloc(acc_element->accumulator, accumulator_size + data_size + 1);
    strcat(acc_element->accumulator, test_data->buffer);
}

/**
 * Reserves accumulator memory
 */
static ArrayElement *reduce_allocate_memory(void *data) {
    ArrayElement *element = calloc(sizeof(struct ArrayElement), 1);
    element->accumulator = calloc(sizeof(char), strlen(data) + 1);
    strcpy(element->accumulator, data);
    return element;
}

static void reduce_free_memory(ArrayElement *element) {
    if (element) {
        if (element->accumulator) {
            free(element->accumulator);
            element->accumulator = NULL;
        }
        element->free_memory = NULL;
        free(element);
        element = NULL;
    }
}

static int reduce_strings() {
    struct memory_functions memory_functions = {
            .allocate_memory = reduce_allocate_memory,
            .free_memory = reduce_free_memory,
            .map_allocate_memory = NULL
    };
    ArrayElement *reduced_data = filtered_array->reduce(filtered_array, reducer, "", &memory_functions);
    bool result = !strcmp(reduced_data->accumulator, "my test data 1my test data 3");
    reduced_data->free_memory(reduced_data);
    return result;
}

static bool find_test_data_7(ArrayElement *element) {
    TestData *test_data = element->data;
    return !strcmp(test_data->buffer, "my test data 7");
}

static bool find_non_existing_data(ArrayElement *element) {
    TestData *test_data = element->data;
    return !strcmp(test_data->buffer, "no data found");
}

static int find_element() {
    ArrayElement *element = array->find(array, find_test_data_7);
    TestData *test_data = element->data;
    return !strcmp(test_data->buffer, "my test data 7");
}

static int element_not_found() {
    ArrayElement *element = array->find(array, find_non_existing_data);
    return element == NULL;
}

static int destroy_arrays() {
    mapped_array = array_quit(mapped_array);
    filtered_array = array_quit(filtered_array);
    array = array_quit(array);
    big_array = array_quit(big_array);
    return array == NULL && mapped_array == NULL && filtered_array == NULL && big_array == NULL;
}

ArrayElement *allocate_number(void *number) {
    ArrayElement *element = calloc(sizeof(struct ArrayElement), 1);
    element->data = calloc(sizeof(size_t), 1);
    *((size_t *) element->data) = *((size_t *) number);
    return element;
}

void free_number(ArrayElement *element) {
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

static uint64_t get_posix_clock_time() {
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0) {
        return (uint64_t) (ts.tv_sec * 1000000 + ts.tv_nsec / 1000);
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
    big_array = array_init(&memory_functions);
    uint64_t prev_time_value, time_value;
    prev_time_value = get_posix_clock_time();
    for (size_t i = 0; i < 1000000; i++) {
        big_array->add(big_array, &i);
    }
    time_value = get_posix_clock_time();
    float time_diff = (float) (time_value - prev_time_value) / 1000000;
    fprintf(stdout, "Inserted a million records in %lf seconds\n", time_diff);
    return big_array->length == 1000000;
}

int main(void) {
    run_test("creating array", create_array);
    run_test("adding elements", adding_elements);
    run_test("filtering elements", filtering_elements);
    run_test("adding elements in filtered array", adding_elements_in_filtered_array);
    run_test("removing elements in filtered array", removing_elements_in_filtered_array);
    run_test("removing elements", removing_elements);
    run_test("mapping_elements", mapping_elements);
    run_test("adding elements in mapped array", adding_elements_in_mapped_array);
    run_test("removing elements in mapped array", removing_elements_in_mapped_array);
    run_test("inserting a million records", inserting_a_million_records);
    run_test("find_element", find_element);
    run_test("element_not_found", element_not_found);
    run_test("reduce_strings", reduce_strings);
    run_test("destroying arrays", destroy_arrays);
    show_tests_result;
    return 0;
}
