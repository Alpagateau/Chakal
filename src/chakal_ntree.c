#include "chakal_ntree.h"
#include "chakal_allocator.h"


struct chakal_ntree* chakal_ntree_of(struct chakal_arena*  a, void* ptr)
{
  struct chakal_ntree* tree = chakal_alloc(a, sizeof(void*));
  tree->value = ptr;
  tree->parent = NULL;
  return tree;
}

struct chakal_ntree* chakal_ntree_append(
  struct chakal_arena* a,
  struct chakal_ntree* tree, void* d
)
{
  struct chakal_ntree* nt = chakal_ntree_of(a, d);
  if(tree != NULL)
  {
    nt->parent = tree;
  }
  return nt;
}

size_t chakal_ntree_read_arguments(
  struct chakal_ntree* t, 
  void** buffer, size_t expected_size
)
{
  size_t current_idx = expected_size-1;
  size_t read_len = 0;
  struct chakal_ntree* current = t;
  do{ 
    buffer[current_idx] = current->value;
    current = current->parent;
    read_len++;
  }while(current->parent != NULL);
  return read_len;
}
