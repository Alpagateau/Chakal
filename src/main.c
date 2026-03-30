#include <math.h>
#include <stdio.h>
#include "chakal.h"

GEN_CLOSURE_APPLY(int, int)
GEN_CLOSURE_APPLY(double, double)

//First order curried function
int add_mul(int a, int b, int c)
{
  return a + b * c;
}

void curried_add_mul(void* result, void** args, struct chakal_arena* env)
{
  (void)env;
  *((int*)result) = add_mul(
    *((int*)args[0]),
    *((int*)args[1]),
    *((int*)args[2])
  );
}

//Second order curried function
void curried_interpolate(void* result, void** args, struct chakal_arena* env)
{
  struct chakal_closure* f = args[0];
  double value = *(double*)args[1];
  int a = floor(value);
  int b = a+1;
  double t = value - a; 
  int x1 = 0;
  int x2 = 0;
  struct chakal_closure* fa = chakal_closure_apply(f, &a);
  struct chakal_closure* fb = chakal_closure_apply(f, &b);
  chakal_closure_eval(fa, &x1);
  chakal_closure_eval(fb, &x2);
  *(double*)result = x2 * t + x1 * (1 - t);
}

int main()
{

  struct chakal_arena* arena = new_arena(1024);
  struct chakal_closure closed_add_mul = 
    {
      .fn = curried_add_mul,
      .arity = 3,
      .applied = 0,
      .args = NULL,
      .alloc = arena
    };

  struct chakal_closure closed_interp = 
    {
      .fn = curried_interpolate,
      .arity = 2,
      .applied = 0,
      .args = NULL,
      .alloc = arena
    };

  int result = 0;
  printf("No packed application : 728/1024\n");
  //struct chakal_closure* add_mul_c = chakal_closure_apply_int(chakal_closure_apply_int(&closed_add_mul,1),2);
  struct chakal_closure* add_mul_c = chakal_closure_apply_multiple(&closed_add_mul, "ii", 1, 2);
  chakal_closure_eval(chakal_closure_apply_int(add_mul_c, 3),&result);
  printf("1 + 2 * 3 = %d\n", result); 
  chakal_closure_eval(chakal_closure_apply_int(add_mul_c, 4),&result);
  printf("1 + 2 * 4 = %d\n", result);

  double r = 0;
  chakal_closure_eval(chakal_closure_apply_multiple(&closed_interp, "*d", add_mul_c, 3.8),&r);
  printf("1 + 2 * 3.8 = %f\n", r); 
  chakal_closure_eval(chakal_closure_apply_multiple(&closed_interp, "*d", add_mul_c, 4.2),&r);
  printf("1 + 2 * 4.2 = %f\n", r); 
  chakal_free_arena(arena);
  return 0;
}
