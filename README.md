# CHAKAL
CHAKAL is a C library making functional programming possible in C.

# CURRENT USAGE
The main type of CHAKAL is the `struct chakal_closure`, which contains both a function pointer, and an access to the arguments already passed to it.
```c
#include <stdio.h>

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

int main()
{
    //Needed for allocation purposes. Here 1M is enough
    struct chakal_arena* arena = new_arena(1024);
    
    //Here we define the closure
    struct chakal_closure closed_add_mul = 
    {
      .fn = curried_add_mul,
      .arity = 3,
      .applied = 0,
      .args = NULL,
      .alloc = arena
    };
    int result = 0;

    //You can save partial applications to variables
    struct chakal_closure* add_mul_c = chakal_closure_apply_multiple(&closed_add_mul, "ii", 1, 2);
    chakal_closure_eval(chakal_closure_apply_int(add_mul_c, 4),&result);
    // result == 9
    return 0;
}
```
To see some examples, check `main.c`. 
