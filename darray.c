#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "darray.h"

typedef long long int lld;

static lld DEFAULT_INITIAL_SIZE = 16;

static bool index_out_of_bounds(lld index, lld length);
static lld modulo_index(lld index, lld length);

int darray_init(DArray *array, lld initial_size, void (*destroy)(void *data))
{
    if (initial_size == 0) {
        initial_size = DEFAULT_INITIAL_SIZE;
    }
    array->arr = calloc(initial_size, sizeof (void *));
    if (array->arr == NULL) {
        return -1;
    }

    array->capacity = initial_size;
    array->len = 0;
    array->destroy = destroy;

    return 0;
}

int darray_copy(DArray *to, DArray *from)
{
    int length = from->len;
    int retval = darray_init(to, length, from->destroy);
    if (retval != 0) {
        return -1;
    }

    to->len = length;
    for (int i = 0; i < length; i++) {
        to->arr[0] = from->arr[0];
    }

    return 0;
}

void darray_destroy(DArray *array)
{
    if (array->destroy != NULL) {
        for (int i = 0; i < array->len; i++) {
            array->destroy(array->arr[i]);
        }
    }
    free(array->arr);
    memset(array, 0, sizeof(DArray));
}

int darray_push(DArray *array, void *datum)
{
    if (array->len == array->capacity) {
        lld new_capacity = (3 * array->capacity) / 2;
        // because (3*1)/2 = 1 in integer division
        // make sure a capacity of 1 actually grows
        new_capacity = new_capacity < 2 ? 2 : new_capacity;
        void **tmp = realloc(array->arr, sizeof (void *) * new_capacity);
        if (tmp == NULL) {
            return -1;
        } else {
            array->arr = tmp;
            array->capacity = new_capacity;
        }
    }

    array->arr[array->len++] = datum;
    return 0;
}

int darray_pop(DArray *array, void **datum)
{
    if (array->len == 0) return -1;

    *datum = array->arr[array->len - 1];
    array->len--;

    if (array->len <= (array->capacity / 2)) {
        lld new_capacity = (2 * array->capacity) / 3;
        new_capacity = new_capacity < 1 ? 1 : new_capacity;
        void **tmp = realloc(array->arr, sizeof (void *) * new_capacity);
        if (tmp == NULL) {
            return -1;
        } else {
            array->arr = tmp;
            array->capacity = new_capacity;
        }
    }
    
    return 0;
}

int darray_get(DArray *array, void **datum, lld index)
{
    if (index_out_of_bounds(index, array->len)) {
        return -1;
    }

    index = modulo_index(index, array->len);
    *datum = array->arr[index];
    return 0;
}

int darray_slice(DArray *dest, DArray *source, lld start, lld end)
{
    if (index_out_of_bounds(start, source->len)
        || index_out_of_bounds(end, source->len + 1)) return -2;

    // Turn negative indices into true ones if needs be
    // Respecting that end of slice can be one past the final index
    lld m_start = modulo_index(start, source->len);
    lld m_end = end == source->len ? end : modulo_index(end, source->len);
    
    lld range = m_end - m_start;
    if (m_start >= m_end) return -3; // Slice must be in order


    int retval = darray_init(dest, range, source->destroy);
    if (retval != 0) return -1;

    for (lld i = 0; i < range; i++) {
        dest->arr[i] = source->arr[m_start + i];
        dest->len++;
    }
    return 0;
}

static bool index_out_of_bounds(lld index, lld length)
{
    return index > length - 1 || llabs(index) > length;
}

static lld modulo_index(lld index, lld length)
{
    lld i = index % length;
    if (i < 0) i += length;
    return i;
}
