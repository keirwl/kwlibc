#include <stdlib.h>

typedef struct DArray_ {
    void (*destroy)(void *data);
    long long int capacity;
    long long int len;
    void **arr;
} DArray;

DArray *darray_init(long long int initial_num, void (*destroy)(void *data));

DArray *darray_copy(DArray *array);

void darray_destroy(DArray *array);

// Possibly resizes the array, so returns 0 for success, -1 for failure.
int darray_push(DArray *array, void *datum);

// Possibly resizes the array, so returns 0 for success, -1 for failure.
int darray_pop(DArray *array, void **datum);

// Gets value but doesn't resize array. Accepts negative indices!
// Returns 0 for success, -1 for out-of-bounds error.
int darray_get(DArray *array, void **datum, long long int index);

// Returns a copy of the array from [start, end). Accepts negative indices,
// Expects caller to make sure start and end are in order and not equal, or returns NULL
DArray *darray_slice(DArray *array, long long int start, long long int end);

#define darray_len(array) ((array)->len)

#define darray_capacity(array) ((array)->capacity)
