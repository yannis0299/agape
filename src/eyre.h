#ifndef EYRE_H
#define EYRE_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "prelude.h"

/* Eyre manipulation */
#define EYRE_MAX_MESSAGE_LENGTH 2048

typedef struct {
  char *filename;
  usize line;
  char *message;
} eyre_t;

static inline eyre_t eyre_new(const char *filename, const usize line,
                              const char *fmt, ...) {
  eyre_t err = {
      .filename = (char *)filename,
      .line = line,
      .message = (char *)malloc(EYRE_MAX_MESSAGE_LENGTH),
  };
  va_list va_args;
  va_start(va_args, fmt);
  vsnprintf((char *)err.message, EYRE_MAX_MESSAGE_LENGTH, fmt, va_args);
  va_end(va_args);
  return err;
}

static inline void eyre_panic(eyre_t err) {
  fprintf(stderr, "Eyre: %s:%zu\n\t%s\n", err.filename, err.line, err.message);
  exit(1);
}

#define eyre_bail(fmt, ...)                                                    \
  do {                                                                         \
    eyre_t err = eyre_new(__FILE__, __LINE__, fmt __VA_OPT__(, ) __VA_ARGS__); \
    eyre_panic(err);                                                           \
  } while (0);

#endif // EYRE_H
