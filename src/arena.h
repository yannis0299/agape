#ifndef ARENA_H
#define ARENA_H

#include "prelude.h"

typedef struct {
  u8 *start, *end;
  usize capacity, size;
} global_arena_t;

extern global_arena_t *GLOBAL_ARENA;

void ga_init(usize heap_size);

void ga_destroy();

void ga_reset();

void *ga_alloc(usize size);

void *ga_realloc(void *old_ptr, usize old_size, usize new_size);

#endif // ARENA_H
