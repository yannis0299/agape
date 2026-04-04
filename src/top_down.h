#ifndef TOP_DOWN_H
#define TOP_DOWN_H

#include "arena.h"
#include "generics.h"
#include "prelude.h"
#include "tokenizer.h"
#include "translation_unit.h"

typedef enum {
  STMT_PRAGMA,
  STMT_MATCH_CASE,
  STMT_BIND_EXPR,
} stmt_kind_t;

typedef struct {
  stmt_kind_t kind;
  slice_token_t lhs;
  slice_token_t rhs;
} stmt_t;

impl_generics(stmt_t, stmt);

typedef enum {
  BLOCK_TOPLEVEL,
  BLOCK_MATCH,
  BLOCK_WHERE,
} block_kind_t;

typedef struct {
  block_kind_t kind;
  usize column;
  vec_stmt_t stmts;
} block_t;

impl_generics(block_t, block);

typedef struct {
  TU_t *tu;
  slice_token_t tokens;
  vec_block_t stack;
} top_down_t;

#endif // TOP_DOWN_H
