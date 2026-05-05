#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chakal_linked_list.h"

struct chakal_linked_list
{
  struct chakal_arena* arena;
  void* data;
  size_t size;
  struct chakal_linked_list* next;
};

struct chakal_linked_list* empty_chakal_linked_list_list(struct chakal_arena* arena)
{
  struct chakal_linked_list* l = malloc(sizeof(*l));
  l->arena = arena;
  l->data = NULL;
  l->size = 0;
  l->next = NULL;
  return l;
}

bool is_empty(const struct chakal_linked_list* l)
{
  return (l->data == NULL) || (l->size == 0);
}

void push_back(struct chakal_linked_list* l, size_t s, void* d)
{
  if(l->next != NULL)
  {
    push_back(l->next, s, d);
  }
  else 
  {
    struct chakal_linked_list* l2 = empty_chakal_linked_list_list(l->arena);
    printf("[DEBUG] Push back element (linked list)\n");
    l2->data = chakal_alloc(l->arena,s);
    l2->size = s;
    memcpy(l2->data, d, s);
    l->next = l2;
  }
}

struct chakal_linked_list* push_front(struct chakal_linked_list* l, size_t s, void* d)
{
  struct chakal_linked_list* l2 = empty_chakal_linked_list_list(l->arena);
  l2->data = chakal_alloc(l->arena, s);
  l2->size = s;
  memcpy(l2->data, d, s);
  if(!is_empty(l))
    l2->next = l; 
  return l2;
}

struct chakal_linked_list* pop_front(struct chakal_linked_list* l)
{
  if(is_empty(l))
  {
    free(l);
    return NULL;
  }

  struct chakal_linked_list* n = l->next;
  free(l->data);
  free(l);
  return n;
}

void* first(struct chakal_linked_list* l, size_t* size_out)
{
  if(is_empty(l))
  {
    *size_out = 0;
    return NULL;
  }
  *size_out = l->size;
  return l->data;
}

void* get_at(struct chakal_linked_list* l, int idx, size_t* size_out)
{
  if(is_empty(l) || (l->next == NULL && idx > 0) || idx < 0)
  {
    *size_out = 0;
    return NULL;
  }

  if(idx == 0)
    return first(l, size_out);
  else
    return get_at(l->next, idx-1, size_out);
}
