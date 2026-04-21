#include "chakal_types.h"
#include "chakal_ntree.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

struct chakal_closure *chakal_closure_apply(struct chakal_closure *cl, void *arg) {
  
  if(cl == NULL) return NULL;
  if(cl->kind == ATOM) return cl;

  struct chakal_closure *new_cl = chakal_alloc(cl->partial.alloc, sizeof(*new_cl));
  new_cl->partial.fn = cl->partial.fn;
  new_cl->partial.arity = cl->partial.arity;
  new_cl->partial.applied = cl->partial.applied + 1;
  // TODO
  new_cl->partial.alloc = cl->partial.alloc;
  //new_cl->args = chakal_alloc(new_cl->alloc, sizeof(void *) * new_cl->applied);
  new_cl->partial.args = chakal_ntree_append(
    cl->partial.alloc,cl->partial.args, arg
  );
  //Collapse functions when arity is filled
  if(new_cl->partial.arity == new_cl->partial.applied)
  {
    return chakal_closure_eval(new_cl);
  }
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
  struct chakal_closure *new_cl = chakal_alloc(cl->partial.alloc, sizeof(*new_cl));
  new_cl->partial.fn = cl->partial.fn;
  new_cl->partial.arity = cl->partial.arity;
  new_cl->partial.applied = cl->partial.applied + arg_number;
  new_cl->partial.alloc = cl->partial.alloc;
  new_cl->partial.args = cl->partial.args;
  
  size_t fmt_idx = 0;
  while (fmt[fmt_idx] != 0) {
    switch (fmt[fmt_idx]) {
    case 'c':
      {
          char* c = chakal_alloc(cl->partial.alloc, sizeof(char));
          *c = (char)va_arg(args, int);
          new_cl->partial.args = chakal_ntree_append(cl->partial.alloc, new_cl->partial.args, c);
      }
      break;
    case 'i':
      {
          int* c = chakal_alloc(cl->partial.alloc, sizeof(int));
          *c = (int)va_arg(args, int);
          new_cl->partial.args = chakal_ntree_append(cl->partial.alloc, new_cl->partial.args, c);
      }
      break;
    case 'f':
      {
          float* c = chakal_alloc(cl->partial.alloc, sizeof(float));
          *c = (float)va_arg(args, double);
          new_cl->partial.args = chakal_ntree_append(cl->partial.alloc, new_cl->partial.args, c);
      }
      break;
    case 'd':
      {
          double* c = chakal_alloc(cl->partial.alloc, sizeof(double));
          *c = (double)va_arg(args, double);
          new_cl->partial.args = chakal_ntree_append(cl->partial.alloc, new_cl->partial.args, c);
      }
      break;
    case '*':
      {
          new_cl->partial.args = chakal_ntree_append(cl->partial.alloc, new_cl->partial.args, va_arg(args, void*));
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

struct chakal_closure* chakal_closure_apply_eval(
  struct chakal_closure* cl, void* arg
)
{ 
  struct chakal_arena* a = new_arena(256);
  struct chakal_closure *new_cl = chakal_alloc(a, sizeof(*new_cl));
  new_cl->partial.fn = cl->partial.fn;
  new_cl->partial.arity = cl->partial.arity;
  new_cl->partial.applied = cl->partial.applied + 1;
  new_cl->partial.alloc = a;
  new_cl->partial.args = chakal_ntree_append(
    a,cl->partial.args,arg
  );
  struct chakal_closure* b = chakal_closure_eval(new_cl);
  chakal_free_arena(a);
  return b;
}


struct chakal_closure* chakal_closure_eval(struct chakal_closure *cl) {

  if(cl->kind == ATOM) return cl;
  if ( cl->partial.fn == NULL || cl->partial.applied != cl->partial.arity ) {
    return cl;
  }

  void** args = malloc(sizeof(void*) * (cl->partial.arity));
  size_t argn = chakal_ntree_read_arguments(cl->partial.args, args, cl->partial.arity);
  (void)argn;
  struct chakal_closure* atom = chakal_alloc(cl->partial.alloc, sizeof(struct chakal_closure));
  atom->kind = ATOM; 
  
  cl->partial.fn(&(atom->atom.data), args, cl->partial.alloc);
   
  free(args);
  return atom;
}
