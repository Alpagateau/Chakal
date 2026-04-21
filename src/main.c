#include <math.h>
#include <stdio.h>
#include "chakal.h"
#include "chakal_allocator.h"
#include "chakal_types.h"

GEN_CLOSURE_APPLY(int, int)
//GEN_CLOSURE_APPLY(double, double)

//First order curried function
static int add_mul(int _a, int _b, int _c)
{
  return _a + (_b * _c);
}

static void curried_add_mul(void** result, void** args, struct chakal_arena* env)
{
  (void)env;
  int res = add_mul(
    *((int*)args[0]),
    *((int*)args[1]),
    *((int*)args[2])
  );
  *result = chakal_alloc(env, sizeof(int));
  *(int*)(*result) = res;
}

//Second order curried function
static void curried_interpolate(void** result, void** args, struct chakal_arena* env)
{
  struct chakal_closure* func = args[0];
  double value = *(double*)args[1];
  int min = floor(value);
  int max = min+1;
  double percent = value - min; 
  struct chakal_closure* fmin = chakal_closure_apply(func, &min);
  struct chakal_closure* fmax = chakal_closure_apply(func, &max);
  struct chakal_closure* less = chakal_closure_eval(fmin);
  struct chakal_closure* more = chakal_closure_eval(fmax);
  int x_1 = *(int*)(less->atom.data);
  int x_2 = *(int*)(more->atom.data);
  double res = (x_2 * percent) + (x_1 * (1 - percent));
  *result = chakal_alloc(env, sizeof(double));
  *(double*)(*result) = res;
}

int main()
{
  struct chakal_arena* arena = new_arena(1024);
  struct chakal_closure closed_add_mul = 
    {
      .kind = PARTIAL,
      .partial.fn = curried_add_mul,
      .partial.arity = 3,
      .partial.applied = 0,
      .partial.args = NULL,
      .partial.alloc = arena
    };

  struct chakal_closure closed_interp = 
    {
      .kind = PARTIAL,
      .partial.fn = curried_interpolate,
      .partial.arity = 2,
      .partial.applied = 0,
      .partial.args = NULL,
      .partial.alloc = arena
    };

  printf("No packed application : 728/1024\n");
  printf("With packed application : 524/1024\n");
  //struct chakal_closure* add_mul_c = chakal_closure_apply_int(chakal_closure_apply_int(&closed_add_mul,1),2);
  struct chakal_closure* add_mul_c = chakal_closure_apply_multiple(&closed_add_mul, "ii", 1, 2);
  struct chakal_closure* res = chakal_closure_eval(
    chakal_closure_apply_int(add_mul_c, 3)
  );
  
  printf("1 + 2 * 3 = %d\n", *(int*)res->atom.data); 
  int four = 4;
  res = chakal_closure_apply_eval(add_mul_c, &four);
  printf("1 + 2 * 4 = %d\n", *(int*)res->atom.data);
  
  double data = 0;
  res = chakal_closure_eval(chakal_closure_apply_multiple(&closed_interp, "*d", add_mul_c, 3.8));
  data = *(double*)res->atom.data;
  printf("1 + 2 * 3.8 = %f\n", data); 
  res = chakal_closure_eval(chakal_closure_apply_multiple(&closed_interp, "*d", add_mul_c, 4.2));
  data = *(double*)res->atom.data;
  printf("1 + 2 * 4.2 = %f\n", data);
  chakal_free_arena(arena);
  return 0;
}
