#include "str.h"

#include <string.h>

#include "arena.h"

str_t str_new(usize cap) {
  cap = cap ? cap : 1;
  u8 *raw = (u8 *)ga_alloc(cap);
  raw[0] = '\0';
  return (str_t){
      .cap = cap,
      .len = 0,
      .raw = raw,
  };
}

str_t str_from(const char *src) {
  usize len = strlen(src);
  str_t self = str_new(len + 1);
  strncpy((char *)self.raw, src, len);
  self.len = len;
  self.raw[self.len] = '\0';
  return self;
}

void str_push(str_t *self, char c) {
  if (self->len + 1 >= self->cap) {
    usize cap = self->cap ? 2 * self->cap : 1;
    self->raw = (u8 *)ga_realloc(self->raw, self->cap, cap);
    self->cap = cap;
  }
  self->raw[self->len++] = (u8)c;
  self->raw[self->len] = '\0';
}

void str_extend(str_t *self, const u8 *part, usize len) {
  if (len + self->len + 1 >= self->cap) {
    usize cap = self->cap ? self->cap : 1;
    while (len + self->len + 1 >= cap)
      cap *= 2;
    self->raw = (u8 *)ga_realloc(self->raw, self->cap, cap);
    self->cap = cap;
  }
  strncpy((char *)(self->raw + self->len), (char *)part, len);
  self->len += len;
  self->raw[self->len] = '\0';
}
