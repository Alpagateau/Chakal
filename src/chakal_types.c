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

  size_t computed_new_size = 0;
  size_t arg_number = 0;
  chakal_format_allocated_size(fmt, &computed_new_size, &arg_number);
  void *buffer = chakal_alloc(cl->alloc, computed_new_size);
  struct chakal_closure *new_cl = chakal_alloc(cl->alloc, sizeof(*new_cl));
  new_cl->fn = cl->fn;
  new_cl->arity = cl->arity;
  new_cl->applied = cl->applied + arg_number;
  new_cl->alloc = cl->alloc;
  new_cl->args = chakal_alloc(new_cl->alloc, sizeof(void *) * new_cl->applied);
  for (size_t i = 0; i < cl->applied; i++)
    new_cl->args[i] = cl->args[i];

  size_t fmt_idx = 0;
  size_t arg_idx = 0;
  size_t buffer_idx = 0;
  while (fmt[fmt_idx] != 0 && arg_idx < arg_number && buffer_idx < computed_new_size) {
    new_cl->args[cl->applied + arg_idx] = &((char *)buffer)[buffer_idx];
    switch (fmt[fmt_idx]) {
    case 'c':
      *(char *)((char *)buffer + buffer_idx) = (char)va_arg(args, int);
      buffer_idx += sizeof(char);
      break;
    case 'i':
      *(int *)((char *)buffer + buffer_idx) = va_arg(args, int);
      buffer_idx += sizeof(int);
      break;
    case 'f':
      *(float *)((char *)buffer + buffer_idx) = (float)va_arg(args, double);
      buffer_idx += sizeof(float);
      break;
    case 'd':
      *(double *)((char *)buffer + buffer_idx) = va_arg(args, double);
      buffer_idx += sizeof(double);
      break;
    case '*':
      new_cl->args[cl->applied+arg_idx] = va_arg(args, void *);
      break;
    default:
        arg_idx--;
    }
    fmt_idx++;
    arg_idx++;
  }
  va_end(args);
  return new_cl;
}

void chakal_closure_eval(struct chakal_closure *cl, void *result) {
  if (cl->applied != cl->arity) {
    result = NULL;
  }
  cl->fn(result, cl->args, cl->alloc);
}
