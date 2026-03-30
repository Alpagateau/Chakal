#ifndef CHAKAL_LINKED_LIST_H
#define CHAKAL_LINKED_LIST_H

#include <stddef.h>
#include <stdbool.h>

#include "chakal_allocator.h"

struct chakal_linked_list;

struct chakal_linked_list* empty_chakal_linked_list_list(struct chakal_arena*);
bool is_empty(const struct chakal_linked_list*);
void push_back(struct chakal_linked_list* l, size_t s, void* d);
struct chakal_linked_list* push_front(struct chakal_linked_list* l, size_t s, void* d);
struct chakal_linked_list* pop_front(struct chakal_linked_list* l);
void* first(struct chakal_linked_list* l, size_t* size_out);
void* get_at(struct chakal_linked_list* l, int idx, size_t* size_out);

#endif //CHAKAL_LINKED_LIST_H
