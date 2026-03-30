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
  new_cl->alloc = cl->alloc; \
  t *ptr = chakal_alloc(cl->alloc, sizeof(t)); \
  *ptr = arg; \
  new_cl->args = chakal_ntree_append( \
    cl->alloc,cl->args,ptr \
  ); \
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
