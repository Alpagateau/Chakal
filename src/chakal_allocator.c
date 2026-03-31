#include "chakal_allocator.h"
#include <stdio.h>
#include <stdlib.h>

struct chakal_arena *new_arena(size_t size) {
  struct chakal_arena *a = malloc(sizeof(*a));
  a->arena = malloc(sizeof(char) * size);
  a->size = 0;
  a->capacity = size;
  return a;
}

void *chakal_alloc(struct chakal_arena *a, size_t size) {
  if(a == NULL) return NULL;
  if (a->size + size >= a->capacity)
    return NULL;
  void *ptr = &a->arena[a->size];
  a->size += size;
  //printf("Arena <%zu / %zu>\n", a->size, a->capacity);
  return ptr;
}

void chakal_free_arena(struct chakal_arena *a) {
  free(a->arena);
  free(a);
}
