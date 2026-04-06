#ifndef TOP_DOWN_H
#define TOP_DOWN_H

#include <stdio.h>

#include "arena.h"
#include "generics.h"
#include "prelude.h"
#include "str.h"
#include "tokenizer.h"
#include "translation_unit.h"

typedef enum {
  BLOCK_TOP_LEVEL,
  BLOCK_BIND_EXPR,
  BLOCK_PRAGMA,
  BLOCK_LEAF,
  BLOCK_MAX_KIND,
} block_kind_t;

static const char *BLOCK_PRINT_TABLE[BLOCK_MAX_KIND] = {
    "BLOCK_TOP_LEVEL",
    "BLOCK_BIND_EXPR",
    "BLOCK_PRAGMA",
    "BLOCK_LEAF",
};

typedef struct _block {
  block_kind_t kind;
  vec_token_t lhs;
  struct _block *rhs;
} block_t;

void display_block(block_t *self, fmt_t *fmt);

impl_generics(block_t, block);

typedef struct {
  TU_t *tu;
  usize pos;
  slice_token_t tokens;
  vec_block_t stack;
} top_down_t;

top_down_t top_down_new_from_tokenizer(tokenizer_t *tokenizer);

token_t *top_down_peek(top_down_t *self);

token_t *top_down_next(top_down_t *self);

void top_down_exhaust(top_down_t *self);

#endif // TOP_DOWN_H
