#include "darray.h"
#include "minunit.h"

#include <stdio.h>

MU_TEST(create_and_destroy) {
    DArray *test = darray_init(0, NULL);
    mu_check(test != NULL);
    darray_destroy(test);
}

MU_TEST(default_initial_capacity) {
    DArray *test = darray_init(0, NULL);
    mu_assert_int_eq(16, test->capacity);
    mu_assert_int_eq(0, test->len);
    darray_destroy(test);
}

MU_TEST(set_initial_capacity) {
    DArray *test = darray_init(5, NULL);
    mu_assert_int_eq(5, test->capacity);
    mu_assert_int_eq(0, test->len);
    darray_destroy(test);
}

MU_TEST(push_pop) {
    DArray *test = darray_init(1, NULL);
    int pushval = 12;
    int *popval;

    mu_check(darray_push(test, &pushval) == 0);
    mu_assert_int_eq(1, test->len);
    mu_check(darray_pop(test, (void**)&popval) == 0);
    mu_assert_int_eq(pushval, *popval);

    // popping from the now-empty list should return an error code
    mu_check(darray_pop(test, (void**)&popval) == -1);
    darray_destroy(test);
}

MU_TEST(grow_capacity) {
    DArray *test = darray_init(1, NULL);
    mu_assert_int_eq(1, test->capacity);
    mu_assert_int_eq(0, test->len);

    // A cool hacky way of getting the memory address
    // of integer literals: https://stackoverflow.com/a/3443883
    darray_push(test, &(int){2});
    mu_assert_int_eq(1, test->capacity);
    mu_assert_int_eq(1, test->len);

    darray_push(test, &(int){3});
    mu_assert_int_eq(2, test->capacity);
    mu_assert_int_eq(2, test->len);

    darray_push(test, &(int){5});
    mu_assert_int_eq(3, test->capacity);
    mu_assert_int_eq(3, test->len);

    darray_push(test, &(int){7});
    mu_assert_int_eq(4, test->capacity);
    mu_assert_int_eq(4, test->len);

    darray_push(test, &(int){11});
    mu_assert_int_eq(6, test->capacity);
    mu_assert_int_eq(5, test->len);

    darray_push(test, &(int){13});
    mu_assert_int_eq(6, test->capacity);
    mu_assert_int_eq(6, test->len);
    darray_destroy(test);
}

MU_TEST(shrink_capacity) {
    DArray *test = darray_init(16, NULL);
    for (int i = 0; i < 16; i++) {
        darray_push(test, &(int){0});
    }
    mu_assert_int_eq(16, test->capacity);
    mu_assert_int_eq(16, test->len);

    int *popval;
    for (int i = 0; i < 8; i++) darray_pop(test, (void **)&popval);
    mu_assert_int_eq(10, test->capacity);
    mu_assert_int_eq(8, test->len);
    darray_destroy(test);
}

MU_TEST(get_by_index) {
    DArray *test = darray_init(4, free);
    for (int i = 0; i < 4; i++) {
        double *tmp = malloc(sizeof tmp); // Why not try some doubles?
        *tmp = i + 0.5;
        darray_push(test, tmp);
        double *out = test->arr[i];
    }
    mu_assert_int_eq(4, test->capacity);
    mu_assert_int_eq(4, test->len);

    double *getval;

    // Check positive indices
    mu_check(darray_get(test, (void**)&getval, 0) == 0);
    mu_assert_double_eq(0.5, *getval);
    mu_check(darray_get(test, (void**)&getval, 2) == 0);
    mu_assert_double_eq(2.5, *getval);

    // Check negative indices
    mu_check(darray_get(test, (void**)&getval, -1) == 0);
    mu_assert_double_eq(3.5, *getval);
    mu_check(darray_get(test, (void**)&getval, -3) == 0);
    mu_assert_double_eq(1.5, *getval);

    // Check out-of-bound indices
    mu_check(darray_get(test, (void**)&getval, 4) == -1);
    mu_check(darray_get(test, (void**)&getval, -5) == -1);
    darray_destroy(test);
}

MU_TEST(slice) {
    int inputs[5] = {2, 3, 5, 7, 11};
    int *i_out;
    DArray *source = darray_init(5, NULL);
    for (int i = 0; i < 5; ++i) {
        darray_push(source, &inputs[i]);
    }

    // Slice a single value
    DArray *one_slice = darray_slice(source, 0, 1);
    mu_check(one_slice != NULL);
    mu_check(darray_len(one_slice) == 1);
    darray_get(one_slice, (void **)&i_out, 0);
    mu_assert_int_eq(2, *i_out);
    darray_destroy(one_slice);

    // Slice the entire darray, i.e. copy it
    DArray *full_slice = darray_slice(source, 0, 5);
    mu_check(full_slice != NULL);
    mu_check(darray_len(one_slice) == 5);
    for (int i = 0; i < 5; ++i) {
        darray_get(full_slice, (void **)&i_out, i);
        mu_assert_int_eq(inputs[i], *i_out);
    }
    darray_destroy(full_slice);

    // Slice with a negative index
    DArray *slice_with_neg = darray_slice(source, 1, -1);
    mu_check(slice_with_neg != NULL);
    mu_check(darray_len(slice_with_neg) == 3);
    int expected[] = {3, 5, 7};
    for (int i = 0; i < 3; ++i) {
        darray_get(slice_with_neg, (void **)&i_out, i);
        mu_assert_int_eq(expected[i], *i_out);
    }
    darray_destroy(slice_with_neg);

    // Slice with start later than end - should fail
    DArray *slice_with_reversed_indices = darray_slice(source, 4, 1);
    mu_check(slice_with_reversed_indices == NULL);

    // Slice that tries to "wrap around" from negative index to start - should fail
    DArray *slice_with_wrap_around = darray_slice(source, -1, 0);
    mu_check(slice_with_wrap_around == NULL);

    darray_destroy(source);
}

MU_TEST_SUITE(darray_test) {
    MU_RUN_TEST(create_and_destroy);
    MU_RUN_TEST(default_initial_capacity);
    MU_RUN_TEST(set_initial_capacity);
    MU_RUN_TEST(push_pop);
    MU_RUN_TEST(grow_capacity);
    MU_RUN_TEST(shrink_capacity);
    MU_RUN_TEST(get_by_index);
    MU_RUN_TEST(slice);
}

int main(int argc, char *argv[])
{
    MU_RUN_SUITE(darray_test);
    MU_REPORT();
    return minunit_status;
}
