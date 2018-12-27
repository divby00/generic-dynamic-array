#include <stdlib.h>
#include <string.h>
#include "vector.h"

typedef struct Data {
    size_t value;
    char* buffer;
} Data;

void* data_malloc(void* data) {
    Data* d = calloc(sizeof(struct Data), 1);
    if (d) {
        d->buffer = calloc(sizeof(char), strlen(data) + 1);
        strcpy(d->buffer, data);
        d->value = 1;
    }
    return d;
}

void data_free(void* data) {
    Data* d = data;
    if (d) {
        if (d->buffer) {
            free(d->buffer);
            d->buffer = NULL;
        }
        free(d);
        d = NULL;
    }
}

int main(void)
{
    Vector* vector = vector_init(data_malloc, data_free);
    vector->add(vector, "1");
    vector->add(vector, "2");
    vector->add(vector, "3");
    for (size_t i=0; i<vector->length; i++) {
        Data* data = vector->get(vector, i);
        fprintf(stdout, "Buffer: %s, Value: %ld\n", data->buffer, data->value);
    }
    vector->remove(vector, 0);
    vector->remove(vector, 0);
    vector->remove(vector, 0);
    vector_quit(vector);
    return 0;
}
