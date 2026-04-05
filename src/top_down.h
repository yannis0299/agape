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
  BLOCK_BIND_EXPR,
  BLOCK_PRAGMA,
  BLOCK_LEAF,
  BLOCK_MAX_KIND,
} block_kind_t;

static const char *BLOCK_PRINT_TABLE[BLOCK_MAX_KIND] = {
    "BLOCK_BIND_EXPR",
    "BLOCK_PRAGMA",
    "BLOCK_LEAF",
};

typedef struct _block {
  block_kind_t kind;
  union {
    struct {
      slice_token_t lhs;
      struct _block *rhs;
    } bind_expr;
    struct {
      slice_token_t args;
    } pragma;
    struct {
      slice_token_t expr;
    } leaf;
  } as;
  struct _block *next;
} block_t;

impl_display(block_t, block) {
  fprintf(fmt->stream, "Block(%-20s, \n", BLOCK_PRINT_TABLE[self->kind]);
  fprintf(fmt->stream, "%*s", (int)fmt->pad, "");
  switch (self->kind) {
  case BLOCK_BIND_EXPR:
    fprintf(fmt->stream, "lhs=%p[%zu], rhs=", self->as.bind_expr.lhs.ptr,
            self->as.bind_expr.lhs.len);
    fmt->pad += 2;
    display_block(self->as.bind_expr.rhs, fmt);
    fmt->pad -= 2;
    break;
  case BLOCK_PRAGMA:
    fprintf(fmt->stream, "args=%p[%zu]\n", self->as.pragma.args.ptr,
            self->as.pragma.args.len);
    break;
  case BLOCK_LEAF:
    fprintf(fmt->stream, "args=%p[%zu]\n", self->as.leaf.expr.ptr,
            self->as.leaf.expr.len);
    break;
  case BLOCK_MAX_KIND:
    break;
  }
  fprintf(fmt->stream, "%*s", (int)fmt->pad, "");
  fprintf(fmt->stream, ")\n");
}

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
