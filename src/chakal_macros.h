#ifndef CHAKAL_MACROS_H
#define CHAKAL_MACROS_H

#define CONCAT(A, B) A##B
#define GEN_CLOSURE_APPLY(name, t) \
static struct chakal_closure* CONCAT(chakal_closure_apply_, name)( \
  struct chakal_closure* cl, t arg \
){ \
  struct chakal_closure* new_cl = chakal_alloc(cl->partial.alloc, sizeof(*new_cl)); \
  new_cl->partial.fn      = cl->partial.fn; \
  new_cl->partial.arity   = cl->partial.arity; \
  new_cl->partial.applied = cl->partial.applied+1; \
  new_cl->partial.alloc   = cl->partial.alloc; \
  t *ptr = chakal_alloc(cl->partial.alloc, sizeof(t)); \
  *ptr = arg; \
  new_cl->partial.args = chakal_ntree_append( \
    cl->partial.alloc,cl->partial.args,ptr \
  ); \
  return new_cl;  \
}

#ifndef CHAKAL_NO_INFO
#define CHAKAL_INFO(A, ...)    printf("[%s:%d]\tINFO\t: " A, __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define CHAKAL_INFO(A, ...)
#endif

#ifndef CHAKAL_NO_WARNING
#define CHAKAL_WARNING(A, ...) printf("[%s:%d]\tWARNING\t: " A, __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define CHAKAL_WARNING(A, ...)
#endif

#ifndef CHAKAL_NO_ERROR
#define CHAKAL_ERROR(A, ...)   printf("[%s:%d]\tERROR\t: " A, __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define CHAKAL_ERROR(A, ...)
#endif

#endif
