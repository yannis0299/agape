#ifndef PRATT_H
#define PRATT_H

#include <stdio.h>

#include "generics.h"
#include "layout.h"
#include "prelude.h"
#include "str.h"
#include "tokenizer.h"
#include "translation_unit.h"

typedef enum {
  EXPR_TOP_LEVEL,
  EXPR_WHERE,
  EXPR_VARIABLE,
  EXPR_LAMBDA,
  EXPR_APPLICATION,
  EXPR_MAX_KIND,
} expr_kind_t;

static const char *EXPR_PRINT_TABLE[EXPR_MAX_KIND] = {
    "EXPR_TOP_LEVEL", "EXPR_WHERE",       "EXPR_VARIABLE",
    "EXPR_LAMBDA",    "EXPR_APPLICATION",
};

typedef struct _expr {
  expr_kind_t kind;
  union {
    struct {
      usize count;
      str_t *idents;
      struct _expr *binds;
    } top_level;
    struct {
      struct _expr *expr;
      usize count;
      str_t *idents;
      struct _expr *binds;
    } where;
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
  usize idx;
  switch (self->kind) {
  case EXPR_TOP_LEVEL:
    fprintf(fmt->stream, "TopLevel({");
    fmt->pad += 2;
    for (idx = 0; idx < self->as.top_level.count; idx++) {
      fprintf(fmt->stream, "\n%*s%s", (int)fmt->pad, "", "ident=");
      fprintf(fmt->stream, "\"%s\",",
              (char *)self->as.top_level.idents[idx].raw);
      fprintf(fmt->stream, "\n%*s%s", (int)fmt->pad, "", "bind=");
      display_expr(&self->as.top_level.binds[idx], fmt);
      if (idx != self->as.top_level.count - 1)
        fprintf(fmt->stream, ";");
    }
    fmt->pad -= 2;
    fprintf(fmt->stream, "})");
    break;
  case EXPR_WHERE:
    fprintf(fmt->stream, "Where(");
    fmt->pad += 2;
    fprintf(fmt->stream, "\n%*s%s", (int)fmt->pad, "", "expr=");
    display_expr(self->as.where.expr, fmt);
    fprintf(fmt->stream, "\n%*s%s", (int)fmt->pad, "", "binds={");
    for (idx = 0; idx < self->as.where.count; idx++) {
      fprintf(fmt->stream, "\n%*s%s", (int)fmt->pad, "", "ident=");
      fprintf(fmt->stream, "\"%s\",", (char *)self->as.where.idents[idx].raw);
      fprintf(fmt->stream, "\n%*s%s", (int)fmt->pad, "", "bind=");
      display_expr(&self->as.where.binds[idx], fmt);
      if (idx != self->as.where.count - 1)
        fprintf(fmt->stream, ";");
    }
    fmt->pad -= 2;
    fprintf(fmt->stream, "})");
    break;
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
    break;
  }
}

impl_generics(expr_t, expr);

typedef struct {
  TU_t *tu;
  tokenizer_t *tokenizer;
  usize pos;
  slice_token_t tokens;
} pratt_t;

pratt_t pratt_new_from_layout(layout_t *layout);

token_t *pratt_peek_token(pratt_t *self);

token_t *pratt_next_token(pratt_t *self);

token_t *pratt_expect_token(pratt_t *self, token_kind_t kind);

expr_t *pratt_parse_atom(pratt_t *self);

expr_t *pratt_parse_atoms(pratt_t *self);

expr_t *pratt_parse_lambda(pratt_t *self);

expr_t *pratt_parse_bindings(pratt_t *self);

expr_t *pratt_parse_expr(pratt_t *self);

expr_t *pratt_parse_top_level(pratt_t *self);

#endif // PRATT_H
