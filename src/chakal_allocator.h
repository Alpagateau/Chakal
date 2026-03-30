#ifndef CHAKAL_ALLOCATOR_H
#define CHAKAL_ALLOCATOR_H
#include <stddef.h>

struct chakal_arena
{
  char* arena;
  size_t capacity;
  size_t size;
};

struct chakal_arena* new_arena(size_t size);
void* chakal_alloc(struct chakal_arena* a, size_t size);
void chakal_free_arena(struct chakal_arena*);

#endif
