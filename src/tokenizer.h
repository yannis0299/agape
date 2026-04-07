#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdio.h>

#include "arena.h"
#include "generics.h"
#include "prelude.h"
#include "str.h"
#include "translation_unit.h"

typedef enum {
  TOKEN_IDENTIFIER,
  TOKEN_LEFT_PAREN,
  TOKEN_RIGHT_PAREN,
  TOKEN_BACKSLASH,
  TOKEN_DOT,
  TOKEN_EOL,
  TOKEN_EOF,
  TOKEN_MAX_KIND,
} token_kind_t;

static const char *TOKEN_PRINT_TABLE[TOKEN_MAX_KIND] = {
    "TOKEN_IDENTIFIER", "TOKEN_LEFT_PAREN", "TOKEN_RIGHT_PAREN",
    "TOKEN_BACKSLASH",  "TOKEN_DOT",        "TOKEN_EOL",
    "TOKEN_EOF",
};

typedef struct {
  token_kind_t kind;
  usize line, column;
  usize start, span;
  str_t repr;
} token_t;

impl_display(token_t, token) {
  fprintf(
      fmt->stream,
      "Token(%-20s, line=%zu, column=%zu, start=%zu, span=%zu, repr=\"%s\")",
      TOKEN_PRINT_TABLE[self->kind], self->line, self->column, self->start,
      self->span, (self->kind != TOKEN_EOL) ? ((char *)self->repr.raw) : "\\n");
}

impl_generics(token_t, token);

typedef struct {
  TU_t *tu;
  usize line, column, pos;
  vec_token_t tokens;
} tokenizer_t;

tokenizer_t tokenizer_new_from_tu(TU_t *tu);

char tokenizer_peek_char(tokenizer_t *self);

char tokenizer_next_char(tokenizer_t *self);

token_t *tokenizer_next(tokenizer_t *self);

void tokenizer_exhaust(tokenizer_t *self);

#endif // TOKENIZER_H
