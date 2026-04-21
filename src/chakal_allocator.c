#include "chakal_allocator.h"
#include <stdlib.h>

struct chakal_arena *new_arena(size_t size) {
  struct chakal_arena *arena = malloc(sizeof(*arena));
  arena->arena = malloc(sizeof(char) * size);
  arena->size = 0;
  arena->capacity = size;
  return arena;
}

void *chakal_alloc(struct chakal_arena *arena, size_t size) {
  if(arena == NULL) { return NULL; }
  if (arena->size + size >= arena->capacity)
  {return NULL;}
  void *ptr = &arena->arena[arena->size];
  arena->size += size;
  return ptr;
}

void chakal_free_arena(struct chakal_arena *a) {
  free(a->arena);
  free(a);
}
