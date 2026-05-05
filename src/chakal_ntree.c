#include "chakal_ntree.h"
#include "chakal_allocator.h"
#include <stdio.h>


struct chakal_ntree* chakal_ntree_of(struct chakal_arena* arena, void* ptr)
{
  printf("[DEBUG] New n-tree\n");
  struct chakal_ntree* tree = chakal_alloc(arena, sizeof(struct chakal_ntree));
  tree->value = ptr;
  tree->parent = NULL;
  return tree;
}

struct chakal_ntree* chakal_ntree_append(
  struct chakal_arena* arena,
  struct chakal_ntree* tree, void* data
)
{
  struct chakal_ntree* new_tree = chakal_ntree_of(arena, data);
  if(tree != NULL)
  {
    new_tree->parent = tree;
  }
  return new_tree;
}

size_t chakal_ntree_read_arguments(
  struct chakal_ntree* tree, 
  void** buffer, size_t expected_size
)
{
  if(buffer == NULL || tree == NULL) return 0;
  int current_idx = expected_size - 1;
  size_t read_len = 0;
  struct chakal_ntree* current = tree;
  do{ 
    buffer[current_idx] = current->value;
    current = current->parent;
    read_len++;
    current_idx--;
  }while(current != NULL && current_idx >= 0 && expected_size - current_idx > 0);
  return read_len;
}
