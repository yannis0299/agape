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
  PRAGMA_INFIX,
  PRAGMA_INTRINSICS,
  PRAGMA_MAX_KIND,
} pragma_kint_t;

static const char *PRAGMA_PRINT_TABLE[PRAGMA_MAX_KIND] = {"PRAGMA_INFIX",
                                                          "PRAGMA_INTRINSICS"};

typedef struct {
  pragma_kint_t kind;
  union as {
    struct infix {
      str_t op;
      i32 assoc;
      i32 prec;
    };
    struct intrinsics {};
  };
} pragma_t;

typedef enum {
  STMT_BIND_EXPR,
  STMT_MATCH_CASE,
  /* Future  */
  // STMT_BIND_TYPE
  // STMT_USE
  // STMT_MOD
  // STMT_DO_THEN
  // STMT_DO_LET
  STMT_MAX_KIND,
} stmt_kind_t;

typedef struct _stmt {
  stmt_kind_t kind;
  union as {
    struct bind_expr {
      slice_token_t lhs;
      slice_token_t rhs;
    };
    struct pragma {
      pragma_kint_t kind;
      struct _stmt *stmt;
    };
  };
} stmt_t;

static const char *STMT_PRINT_TABLE[STMT_MAX_KIND] = {
    "STMT_PRAGMA",
    "STMT_BIND_EXPR",
};

impl_display(stmt_t, stmt) {
  fprintf(fmt->stream, "Stmt(%20s, ", STMT_PRINT_TABLE[self->kind]);
}

impl_generics(stmt_t, stmt);

typedef enum {
  BLOCK_TOP_LEVEL,
  BLOCK_MATCH_WITH,
  BLOCK_WHERE,
  BLOCK_EXPR,
  BLOCK_MAX_KIND,
} block_kind_t;

typedef struct {
  block_kind_t kind;
  slice_token_t expr;
  union as {
    vec_stmt_t stmts;
    struct match {
      slice_token_t expr;
      vec_stmt_t cases;
    };
    slice_token_t expr;
  };
} block_t;

typedef struct {
  TU_t *tu;
  usize pos;
  slice_token_t tokens;
} top_down_t;

#endif // TOP_DOWN_H
