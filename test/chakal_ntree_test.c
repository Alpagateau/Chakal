#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <munit.h>

#include "chakal_allocator.h"
#include "chakal_types.h"
#include "chakal_ntree.h"


static MunitResult test_ntree_append(const MunitParameter params[], void* data)
{
  (void)params; (void)data;
  struct chakal_arena* arena = new_arena(1024);
  int a = 111;
  int b = 222;
  struct chakal_ntree* t = chakal_ntree_append(arena, NULL, &a);
  munit_assert_int(*(int*)t->value, ==, 111);
  munit_assert_ptr_null(t->parent);
  t = chakal_ntree_append(arena, t, &b);
  munit_assert_int(*(int*)t->value, ==, 222);
  munit_assert_ptr_not_null(t->parent);

  chakal_free_arena(arena);
  return MUNIT_OK;
}

static MunitResult test_ntree_extract(const MunitParameter params[], void* data)
{
  (void)params; (void)data;
  struct chakal_arena* arena = new_arena(1024);
  int a = 111;
  int b = 222;
  struct chakal_ntree* t = chakal_ntree_append(arena, NULL, &a);
  munit_assert_int(*(int*)t->value, ==, 111);
  munit_assert_ptr_null(t->parent);
  t = chakal_ntree_append(arena, t, &b);
  munit_assert_int(*(int*)t->value, ==, 222);
  munit_assert_ptr_not_null(t->parent);
  int** arg = malloc(sizeof(int*) * 2);
  size_t size =  chakal_ntree_read_arguments(t, (void**)arg, 2);
  munit_assert_uint(size, ==, 2);
  munit_assert_int(*arg[0], ==, 111);
  munit_assert_int(*arg[1], ==, 222);
  chakal_free_arena(arena);
  return MUNIT_OK;
}

/* -------------------------------------------------------
 * Test Suite Definition
 * -------------------------------------------------------*/

static MunitTest tests[] = {
    { "/ntree/append", test_ntree_append, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    { "/ntree/extract", test_ntree_extract, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
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
