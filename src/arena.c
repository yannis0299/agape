#include "arena.h"

#include <stdlib.h>

#include "eyre.h"

global_arena_t *GLOBAL_ARENA = NULL;

void ga_init(usize heap_size) {
  global_arena_t *arena;
  u8 *heap;

  arena = (global_arena_t *)malloc(sizeof(global_arena_t));
  if (arena == NULL)
    eyre_bail("ga_init: could not allocate global arena meta-object");

  heap = (u8 *)malloc(sizeof(u8) * heap_size);
  if (heap == NULL)
    eyre_bail("ga_init: could not allocate global arena heap");

  arena->size = 0;
  arena->capacity = heap_size;
  arena->start = arena->end = heap;

  GLOBAL_ARENA = arena;
}

void ga_destroy() {
  if (GLOBAL_ARENA) {
    if (GLOBAL_ARENA->start) {
      free(GLOBAL_ARENA->start);
    }
    free(GLOBAL_ARENA);
  }
}

void ga_reset() {
  GLOBAL_ARENA->size = 0;
  GLOBAL_ARENA->end = GLOBAL_ARENA->start;
}

void *ga_alloc(usize size) {
  void *ptr;
  if (GLOBAL_ARENA->size + size > GLOBAL_ARENA->capacity) {
    eyre_bail("ga_alloc: Requested allocation amount "
              "exeeds pool capacity (%zu)\n",
              GLOBAL_ARENA->capacity);
  }
  ptr = GLOBAL_ARENA->end;
  GLOBAL_ARENA->size += size;
  GLOBAL_ARENA->end += size;
  return ptr;
}

void *ga_realloc(void *old_ptr, usize old_size, usize new_size) {
  void *new_ptr;
  new_ptr = ga_alloc(new_size);
  if (old_ptr != NULL)
    memcpy(new_ptr, old_ptr, old_size);
  return new_ptr;
}
