#ifndef GENERICS_H
#define GENERICS_H

#include <stdio.h>

#include "arena.h"
#include "prelude.h"

typedef struct {
  usize pad;
  FILE *stream;
} fmt_t;

#define impl_display(T, name)                                                  \
  static inline void display_##name(T *self, fmt_t *fmt)

#define println(name, arg)                                                     \
  do {                                                                         \
    fmt_t fmt = {.pad = 0, .stream = stdout};                                  \
    display_##name(arg, &fmt);                                                 \
    fprintf(fmt.stream, "\n");                                                 \
  } while (0);

#define _impl_slice(T, name)                                                   \
  typedef struct {                                                             \
    T *ptr;                                                                    \
    usize len;                                                                 \
  } slice_##name##_t;

#define _impl_vec(T, name)                                                     \
  typedef struct {                                                             \
    T *raw;                                                                    \
    usize len, cap;                                                            \
  } vec_##name##_t;                                                            \
                                                                               \
  static inline vec_##name##_t vec_##name##_new(usize cap) {                   \
    return (vec_##name##_t){.raw =                                             \
                                cap ? (T *)ga_alloc(sizeof(T) * cap) : NULL,   \
                            .len = 0,                                          \
                            .cap = cap};                                       \
  }                                                                            \
                                                                               \
  static inline void _vec_##name##_grow_if_needed(vec_##name##_t *self) {      \
    if (self->len >= self->cap) {                                              \
      usize old_cap = self->cap;                                               \
      self->cap = self->cap ? 2 * self->cap : 1;                               \
      self->raw = (T *)ga_realloc(self->raw, sizeof(T) * old_cap,              \
                                  sizeof(T) * self->cap);                      \
    }                                                                          \
  }                                                                            \
                                                                               \
  static inline void vec_##name##_push(vec_##name##_t *self, T elem) {         \
    _vec_##name##_grow_if_needed(self);                                        \
    self->raw[self->len++] = elem;                                             \
  }                                                                            \
                                                                               \
  static inline void vec_##name##_push_unsafe(vec_##name##_t *self, T elem) {  \
    self->raw[self->len++] = elem;                                             \
  }                                                                            \
                                                                               \
  static inline slice_##name##_t vec_##name##_slice(vec_##name##_t *self,      \
                                                    usize start, usize len) {  \
    return (slice_##name##_t){.ptr = &self->raw[start], .len = len};           \
  }                                                                            \
                                                                               \
  static inline slice_##name##_t vec_##name##_as_slice(vec_##name##_t *self) { \
    return (slice_##name##_t){.ptr = self->raw, .len = self->len};             \
  }                                                                            \
                                                                               \
  static inline T *vec_##name##_last(vec_##name##_t *self) {                   \
    return (self->len) ? (self->raw + self->len - 1) : NULL;                   \
  }                                                                            \
                                                                               \
  static inline void display_vec_##name(vec_##name##_t *self, fmt_t *fmt) {    \
    fprintf(fmt->stream, "%*s", (i32)fmt->pad, "");                            \
    fprintf(fmt->stream, "[\n");                                               \
    fmt->pad += 2;                                                             \
    for (usize idx = 0; idx < self->len - 1; idx++) {                          \
      fprintf(fmt->stream, "%*s", (i32)fmt->pad, "");                          \
      display_##name(self->raw + idx, fmt);                                    \
      fprintf(fmt->stream, ",\n");                                             \
    }                                                                          \
    if (self->len > 0) {                                                       \
      fprintf(fmt->stream, "%*s", (i32)fmt->pad, "");                          \
      display_##name(self->raw + self->len - 1, fmt);                          \
      fprintf(fmt->stream, "\n");                                              \
    }                                                                          \
    fmt->pad -= 2;                                                             \
    fprintf(fmt->stream, "%*s", (i32)fmt->pad, "");                            \
    fprintf(fmt->stream, "]\n");                                               \
  }

#define impl_generics(T, name)                                                 \
  _impl_slice(T, name);                                                        \
  _impl_vec(T, name);

#endif // GENERIC_H
