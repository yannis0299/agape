#ifndef TU_H
#define TU_H

#include "arena.h"
#include "prelude.h"
#include "str.h"

#define TU_DEFAULT_CAPACITY 1024

typedef struct {
  str_t filename;
  str_t contents;
} TU_t;

TU_t tu_new_from_file(const char *filename);

#endif // TU_H
