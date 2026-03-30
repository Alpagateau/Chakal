#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <munit.h>

#include "chakal_types.h"

/* -------------------------------------------------------
 * Helpers / Mock Functions
 * -------------------------------------------------------*/

static void sum_ints(void* result, void** args, struct chakal_arena* arena) {
    (void)arena;
    int* out = (int*)result;
    int a = *(int*)args[0];
    int b = *(int*)args[1];
    *out = a + b;
}

static void multiply_double(void* result, void** args, struct chakal_arena* arena) {
    (void)arena;
    double* out = (double*)result;
    double a = *(double*)args[0];
    double b = *(double*)args[1];
    *out = a * b;
}

static void read_int_array(void* result, void** args, struct chakal_arena* a)
{
  (void)a;
  int* array = (int*)args[0];
  int idx = *(int*)args[1];
  *(int*)result = array[idx];
}

/* -------------------------------------------------------
 * Arena Tests
 * -------------------------------------------------------*/

static MunitResult test_arena_basic_alloc(const MunitParameter params[], void* data) {
    (void)params; (void)data;

    struct chakal_arena* arena = new_arena(1024);
    munit_assert_not_null(arena);

    void* ptr1 = chakal_alloc(arena, 100);
    void* ptr2 = chakal_alloc(arena, 200);

    munit_assert_not_null(ptr1);
    munit_assert_not_null(ptr2);
    munit_assert_ptr_not_equal(ptr1, ptr2);

    chakal_free_arena(arena);
    return MUNIT_OK;
}

/* -------------------------------------------------------
 * Closure Tests
 * -------------------------------------------------------*/

static struct chakal_closure* make_sum_closure(struct chakal_arena* arena) {
    struct chakal_closure* cl = chakal_alloc(arena, sizeof(*cl));
    cl->fn = sum_ints;
    cl->arity = 2;
    cl->applied = 0;
    cl->alloc = arena;
    cl->args = NULL;
    return cl;
}

static struct chakal_closure* make_array_closure(struct chakal_arena* arena) {
    struct chakal_closure* cl = chakal_alloc(arena, sizeof(*cl));
    cl->fn = read_int_array;
    cl->arity = 2;
    cl->applied = 0;
    cl->alloc = arena;
    cl->args = NULL;
    return cl;
}

static MunitResult test_closure_single_apply(const MunitParameter params[], void* data) {
    (void)params; (void)data;

    struct chakal_arena* arena = new_arena(1024);
    struct chakal_closure* cl = make_sum_closure(arena);

    int x = 5;
    struct chakal_closure* cl2 = chakal_closure_apply(cl, &x);

    munit_assert_size(cl2->applied, ==, 1);
    munit_assert_ptr_not_null(cl2->args);
    
    chakal_free_arena(arena);
    return MUNIT_OK;
}

static MunitResult test_closure_multiple_apply(const MunitParameter params[], void* data) {
    (void)params; (void)data;

    struct chakal_arena* arena = new_arena(1024);
    struct chakal_closure* cl = make_sum_closure(arena);

    struct chakal_closure* cl2 =
        chakal_closure_apply_multiple(cl, "ii", 3, 7);

    munit_assert_size(cl2->applied, ==, 2);
    chakal_free_arena(arena);
    return MUNIT_OK;
}

static MunitResult test_closure_multiple_ptr(const MunitParameter params[], void* data) {
    (void)params; (void)data;

    struct chakal_arena* arena = new_arena(1024);
    struct chakal_closure* cl = make_array_closure(arena);
    
    int var[4] = {111, 222, 333, 444};
    
    struct chakal_closure* cl2 =
        chakal_closure_apply_multiple(cl, "*i", var, 2);

    munit_assert_size(cl2->applied, ==, 2);
    int result = 0;
    chakal_closure_eval(cl2, &result);
    munit_assert_int(result, ==, 333);

    chakal_free_arena(arena);
    return MUNIT_OK;
}

static MunitResult test_closure_eval_sum(const MunitParameter params[], void* data) {
    (void)params; (void)data;

    struct chakal_arena* arena = new_arena(1024);
    struct chakal_closure* cl = make_sum_closure(arena);

    struct chakal_closure* cl2 =
        chakal_closure_apply_multiple(cl, "ii", 10, 20);

    int result = 0;
    chakal_closure_eval(cl2, &result);

    munit_assert_int(result, ==, 30);

    chakal_free_arena(arena);
    return MUNIT_OK;
}

static MunitResult test_closure_eval_double(const MunitParameter params[], void* data) {
    (void)params; (void)data;

    struct chakal_arena* arena = new_arena(1024);

    struct chakal_closure* cl = chakal_alloc(arena, sizeof(*cl));
    cl->fn = multiply_double;
    cl->arity = 2;
    cl->applied = 0;
    cl->alloc = arena;
    cl->args = NULL;

    struct chakal_closure* cl2 =
        chakal_closure_apply_multiple(cl, "dd", 2.0, 4.0);

    double result = 0.0;
    chakal_closure_eval(cl2, &result);

    munit_assert_double(result, ==, 8.0);

    chakal_free_arena(arena);
    return MUNIT_OK;
}

/* -------------------------------------------------------
 * Edge Cases
 * -------------------------------------------------------*/

static MunitResult test_closure_chaining(const MunitParameter params[], void* data) {
    (void)params; (void)data;

    struct chakal_arena* arena = new_arena(1024);
    struct chakal_closure* cl = make_sum_closure(arena);

    int a = 1, b = 2;

    struct chakal_closure* cl1 = chakal_closure_apply(cl, &a);
    struct chakal_closure* cl2 = chakal_closure_apply(cl1, &b);

    munit_assert_size(cl2->applied, ==, 2);

    int result = 0;
    chakal_closure_eval(cl2, &result);

    munit_assert_int(result, ==, 3);

    chakal_free_arena(arena);
    return MUNIT_OK;
}

static MunitResult test_closure_immutability(const MunitParameter params[], void* data) {
    (void)params; (void)data;

    struct chakal_arena* arena = new_arena(1024);
    struct chakal_closure* cl = make_sum_closure(arena);

    int x = 42;
    struct chakal_closure* cl2 = chakal_closure_apply(cl, &x);

    munit_assert_size(cl->applied, ==, 0);
    munit_assert_size(cl2->applied, ==, 1);

    chakal_free_arena(arena);
    return MUNIT_OK;
}

/* -------------------------------------------------------
 * Test Suite Definition
 * -------------------------------------------------------*/

static MunitTest tests[] = {
    { "/arena/basic_alloc", test_arena_basic_alloc, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },

    { "/closure/single_apply", test_closure_single_apply, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/closure/multiple_apply", test_closure_multiple_apply, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/closure/multiple_pointer", test_closure_multiple_ptr, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/closure/eval_sum", test_closure_eval_sum, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/closure/eval_double", test_closure_eval_double, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },

    { "/closure/chaining", test_closure_chaining, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/closure/immutability", test_closure_immutability, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },

    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite suite = {
    "/chakal",
    tests,
    NULL,
    1,
    MUNIT_SUITE_OPTION_NONE
};

int main(int argc, char* argv[]) {
    return munit_suite_main(&suite, NULL, argc, argv);
}
