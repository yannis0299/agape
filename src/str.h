#ifndef STR_H
#define STR_H

#include "arena.h"
#include "prelude.h"

typedef struct {
  u8 *raw;
  usize len, cap;
} str_t;

str_t str_new(usize cap);

str_t str_from(const char *src);

void str_push(str_t *self, char c);

void str_extend(str_t *self, const u8 *part, usize len);

#endif // STR_H
