#ifndef CHAKAL_TYPES_H
#define CHAKAL_TYPES_H

#include "chakal_allocator.h"
#include "chakal_ntree.h"
#include <stdarg.h>
#include <stddef.h>

//CLOSURE
typedef enum {
  PARTIAL = 0,
  ATOM
} closure_kind_t;

struct chakal_closure
{
  closure_kind_t kind;
  union {
    struct {
      void (*fn)(void** result, void** args, struct chakal_arena*);
      struct chakal_ntree* args;
      size_t arity;
      size_t applied;
      struct chakal_arena* alloc;
    } partial;
    struct {
      void* data;
    } atom;
  };
};

struct chakal_closure* chakal_closure_eval(
  struct chakal_closure* cl
);

struct chakal_closure* chakal_closure_apply(
  struct chakal_closure* cl, void* arg
);

struct chakal_closure* chakal_closure_apply_eval(
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

#endif
