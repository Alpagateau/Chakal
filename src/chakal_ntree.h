#ifndef CHAKAL_NTREE_H
#define CHAKAL_NTREE_H

#include "chakal_allocator.h"

struct chakal_ntree
{
  void* value;
  struct chakal_ntree* parent;
};

struct chakal_ntree* chakal_ntree_of(struct chakal_arena*,void*);
struct chakal_ntree* chakal_ntree_append(
  struct chakal_arena* a,
  struct chakal_ntree*, void*
);
size_t chakal_ntree_read_arguments(
  struct chakal_ntree* t, 
  void** buffer, size_t expected_size
);

#endif
