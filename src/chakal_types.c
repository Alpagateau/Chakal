#include "chakal_types.h"
#include "chakal_ntree.h"
#include <ctype.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

struct chakal_closure *chakal_closure_apply(struct chakal_closure *cl,
                                            void *arg) {
  struct chakal_closure *new_cl = chakal_alloc(cl->alloc, sizeof(*new_cl));
  new_cl->fn = cl->fn;
  new_cl->arity = cl->arity;
  new_cl->applied = cl->applied + 1;
  // TODO
  new_cl->alloc = cl->alloc;
  //new_cl->args = chakal_alloc(new_cl->alloc, sizeof(void *) * new_cl->applied);
  new_cl->args = chakal_ntree_append(
    cl->alloc,cl->args,arg
  );
  //for (size_t i = 0; i < cl->applied; i++)
    //new_cl->args[i] = cl->args[i];
  //new_cl->args[cl->applied] = arg;
  return new_cl;
}

void chakal_format_allocated_size(const char *fmt, size_t *alloc,
                                  size_t *argn) {
  size_t total_size = 0;
  size_t arg_num = 0;
  size_t idx = 0;
  while (fmt[idx] != 0) {
    switch (fmt[idx]) {
    case 'c':
      total_size += sizeof(char);
      arg_num++;
      break;
    case 'i':
      total_size += sizeof(int);
      arg_num++;
      break;
    case 'f':
      total_size += sizeof(float);
      arg_num++;
      break;
    case 'd':
      total_size += sizeof(double);
      arg_num++;
      break;
    case '*':
      total_size += 1;
      arg_num++;
      break;
    }
    idx++;
  }
  *alloc = total_size;
  *argn = arg_num;
}

struct chakal_closure *chakal_closure_apply_multiple(struct chakal_closure *cl,
                                                     const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);

  size_t arg_number = 0;
  size_t a = 0;
  chakal_format_allocated_size(fmt, &a, &arg_number);
  struct chakal_closure *new_cl = chakal_alloc(cl->alloc, sizeof(*new_cl));
  new_cl->fn = cl->fn;
  new_cl->arity = cl->arity;
  new_cl->applied = cl->applied + arg_number;
  new_cl->alloc = cl->alloc;
  new_cl->args = cl->args;
  
  size_t fmt_idx = 0;
  while (fmt[fmt_idx] != 0) {
    switch (fmt[fmt_idx]) {
    case 'c':
      {
          char* c = chakal_alloc(cl->alloc, sizeof(char));
          *c = (char)va_arg(args, int);
          new_cl->args = chakal_ntree_append(cl->alloc, new_cl->args, c);
      }
      break;
    case 'i':
      {
          int* c = chakal_alloc(cl->alloc, sizeof(int));
          *c = (int)va_arg(args, int);
          new_cl->args = chakal_ntree_append(cl->alloc, new_cl->args, c);
      }
      break;
    case 'f':
      {
          float* c = chakal_alloc(cl->alloc, sizeof(float));
          *c = (float)va_arg(args, double);
          new_cl->args = chakal_ntree_append(cl->alloc, new_cl->args, c);
      }
      break;
    case 'd':
      {
          double* c = chakal_alloc(cl->alloc, sizeof(double));
          *c = (double)va_arg(args, double);
          new_cl->args = chakal_ntree_append(cl->alloc, new_cl->args, c);
      }
      break;
    case '*':
      {
          new_cl->args = chakal_ntree_append(cl->alloc, new_cl->args, va_arg(args, void*));
      }
      break;
    default:
      break;
    }
    fmt_idx++;
  }
  va_end(args);
  return new_cl;
}

void chakal_closure_apply_eval(
  struct chakal_closure* cl, void* arg, void* result
)
{ 
  struct chakal_arena* a = new_arena(256);
  struct chakal_closure *new_cl = chakal_alloc(a, sizeof(*new_cl));
  new_cl->fn = cl->fn;
  new_cl->arity = cl->arity;
  new_cl->applied = cl->applied + 1;
  new_cl->alloc = a;
  new_cl->args = chakal_ntree_append(
    a,cl->args,arg
  );
  chakal_closure_eval(new_cl, result);
  chakal_free_arena(a);
}


void chakal_closure_eval(struct chakal_closure *cl, void *result) {
  if (cl->applied != cl->arity) {
    result = NULL;
  }
  void** args = malloc(sizeof(void*) * (cl->arity + 1));
  size_t argn = chakal_ntree_read_arguments(cl->args, args, cl->arity);
  fflush(stdout);
  cl->fn(result, args, cl->alloc);
  free(args);
}
