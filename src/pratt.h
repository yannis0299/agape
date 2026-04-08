#ifndef PRATT_H
#define PRATT_H

#include <stdio.h>

#include "generics.h"
#include "prelude.h"
#include "str.h"
#include "tokenizer.h"
#include "translation_unit.h"

typedef enum {
  EXPR_VARIABLE,
  EXPR_LAMBDA,
  EXPR_APPLICATION,
  EXPR_MAX_KIND,
} expr_kind_t;

static const char *EXPR_PRINT_TABLE[EXPR_MAX_KIND] = {
    "EXPR_VARIABLE",
    "EXPR_LAMBDA",
    "EXPR_APPLICATION",
};

typedef struct _expr {
  expr_kind_t kind;
  union {
    str_t var;
    struct {
      str_t lhs;
      struct _expr *rhs;
    } fn;
    struct {
      struct _expr *lhs;
      struct _expr *rhs;
    } app;
  } as;
} expr_t;

static inline void display_expr(expr_t *self, fmt_t *fmt) {
  switch (self->kind) {
  case EXPR_VARIABLE:
    fprintf(fmt->stream, "Var(\"%s\")", (char *)self->as.var.raw);
    break;
  case EXPR_LAMBDA:
    fprintf(fmt->stream, "Fn(");
    fmt->pad += 2;
    fprintf(fmt->stream, "\n%*s%s", (int)fmt->pad, "", "lhs=");
    fprintf(fmt->stream, "\"%s\",", (char *)self->as.fn.lhs.raw);
    fprintf(fmt->stream, "\n%*s%s", (int)fmt->pad, "", "rhs=");
    if (self->as.fn.rhs)
      display_expr(self->as.fn.rhs, fmt);
    else
      fprintf(fmt->stream, "()");
    fmt->pad -= 2;
    fprintf(fmt->stream, ")");
    break;
  case EXPR_APPLICATION:
    fprintf(fmt->stream, "App(");
    fmt->pad += 2;
    fprintf(fmt->stream, "\n%*s%s", (int)fmt->pad, "", "lhs=");
    if (self->as.app.lhs)
      display_expr(self->as.app.lhs, fmt);
    else
      fprintf(fmt->stream, "()");
    fprintf(fmt->stream, ",");
    fprintf(fmt->stream, "\n%*s%s", (int)fmt->pad, "", "rhs=");
    if (self->as.app.rhs)
      display_expr(self->as.app.rhs, fmt);
    else
      fprintf(fmt->stream, "()");
    fmt->pad -= 2;
    fprintf(fmt->stream, ")");
  case EXPR_MAX_KIND:
    fprintf(fmt->stream, ")");
    break;
  }
}

typedef struct {
  TU_t *tu;
  tokenizer_t *tokenizer;
  usize pos;
  slice_token_t tokens;
} pratt_t;

pratt_t pratt_new_from_tokenizer(tokenizer_t *tokenizer);

token_t *pratt_peek_token(pratt_t *self);

token_t *pratt_next_token(pratt_t *self);

token_t *pratt_expect_token(pratt_t *self, token_kind_t kind);

expr_t *pratt_parse_atom(pratt_t *self);

expr_t *pratt_parse_atoms(pratt_t *self);

expr_t *pratt_parse_expr(pratt_t *self);

#endif // PRATT_H
