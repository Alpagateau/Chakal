#ifndef CHAKAL_TYPES_H
#define CHAKAL_TYPES_H

#include <stddef.h>
#include <stdarg.h>
#include "chakal_allocator.h"
#include "chakal_ntree.h"

#define CONCAT(A, B) A##B
#define GEN_CLOSURE_APPLY(name, t) \
struct chakal_closure* CONCAT(chakal_closure_apply_, name)( \
  struct chakal_closure* cl, t arg \
){ \
  struct chakal_closure* new_cl = chakal_alloc(cl->alloc, sizeof(*new_cl)); \
  new_cl->fn = cl->fn; \
  new_cl->arity = cl->arity; \
  new_cl->applied = cl->applied+1; \
  new_cl->alloc = cl->alloc;  \
  new_cl->args =  \
    chakal_alloc(new_cl->alloc, sizeof(void*) * new_cl->applied); \
  for(size_t i = 0; i < cl->applied; i++)\
  new_cl->args[i] = cl->args[i];\
  t *argp = chakal_alloc(new_cl->alloc, sizeof(t)); \
  *argp = arg;\
  new_cl->args[cl->applied] = argp; \
  return new_cl;  \
}


//CLOSURE
struct chakal_closure
{
  void (*fn)(void* result, void** args, struct chakal_arena*);
  struct chakal_ntree* args;
  size_t arity;
  size_t applied;
  struct chakal_arena* alloc;
};

struct chakal_closure* chakal_closure_apply(
  struct chakal_closure* cl, void* arg
);

void chakal_format_allocated_size(
  const char*, size_t* alloc, size_t* argn);
/// fmt is an array of characters corresponding to each argument's type
/// - c : char
/// - i : int
/// - d : double
/// - f : float
/// - * : void pointer
struct chakal_closure* chakal_closure_apply_multiple(struct chakal_closure* cl, const char* fmt, ...);

void chakal_closure_eval(struct chakal_closure* cl, void* result);
#endif
