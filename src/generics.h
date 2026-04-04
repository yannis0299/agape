#ifndef GENERICS_H
#define GENERICS_H

#include "arena.h"
#include "prelude.h"

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
  }

#define impl_generics(T, name)                                                 \
  _impl_slice(T, name);                                                        \
  _impl_vec(T, name);

#endif // GENERIC_H
