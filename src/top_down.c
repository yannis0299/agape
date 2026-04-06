#include "top_down.h"

#include <stdio.h>

#include "tokenizer.h"

void display_block(block_t *self, fmt_t *fmt) {
  fprintf(fmt->stream, "Block(%s,", BLOCK_PRINT_TABLE[self->kind]);
  fprintf(fmt->stream, "\n%*s%s", (int)fmt->pad, "", "lhs=");
  display_vec_token(&self->lhs, fmt);
  fprintf(fmt->stream, ",\n%*s%s", (int)fmt->pad, "", "rhs=");
  fmt->pad += 2;
  if (self->rhs != NULL)
    display_block(self->rhs, fmt);
  else
    fprintf(fmt->stream, "NULL");
  fmt->pad -= 2;
  fprintf(fmt->stream, ")");
}

top_down_t top_down_new_from_tokenizer(tokenizer_t *tokenizer) {
  vec_block_t stack = vec_block_new(1);
  block_t block = {
      .kind = BLOCK_TOP_LEVEL,
      .lhs = vec_token_new(1),
      .rhs = NULL,
  };
  vec_block_push(&stack, block);
  return (top_down_t){
      .tu = tokenizer->tu,
      .pos = 0,
      .tokens = vec_token_as_slice(&tokenizer->tokens),
      .stack = stack,
  };
}

token_t *top_down_peek(top_down_t *self) {
  if (self->pos >= self->tokens.len) {
    return NULL;
  } else {
    return &self->tokens.ptr[self->pos];
  }
}

token_t *top_down_next(top_down_t *self) {
  if (self->pos >= self->tokens.len) {
    return NULL;
  } else {
    return &self->tokens.ptr[self->pos++];
  }
};

void top_down_exhaust(top_down_t *self) {
  return;
  /* token_t *token = NULL; */
  /* block_t *block = NULL; */
  /* while ((token = top_down_peek(self))) { */
  /*   switch (token->kind) { */
  /*     // Handle reduction */
  /*   case TOKEN_EQUAL: */
  /*     // Consume token */
  /*     top_down_next(self); */
  /*     block = vec_block_last(&self->stack); */
  /*     vec_block_push(&self->stack, (block_t){ */
  /*                                      .kind = BLOCK_BIND_EXPR, */
  /*                                      .lhs = vec_token_new(1), */
  /*                                      .rhs = NULL, */
  /*                                  }); */
  /*     break; */
  /*   case TOKEN_LEFT_PRAGMA: */
  /*   case TOKEN_RIGHT_PRAGMA: */
  /*   case TOKEN_EOL: */

  /*   case TOKEN_EOF: */
  /*   case TOKEN_MAX_KIND: */
  /*     return; */
  /*   // Shift token to top frame */
  /*   case TOKEN_IDENTIFIER: */
  /*   case TOKEN_INTEGER: */
  /*   case TOKEN_OPERATOR: */
  /*   case TOKEN_LEFT_PAREN: */
  /*   case TOKEN_RIGHT_PAREN: */
  /*   case TOKEN_LEFT_BRACKET: */
  /*   case TOKEN_RIGHT_BRACKET: */
  /*   case TOKEN_BACKSLASH: */
  /*   case TOKEN_COMMA: */
  /*   case TOKEN_MATCH: */
  /*   case TOKEN_WITH: */
  /*   case TOKEN_WHERE: */
  /*   case TOKEN_RIGHT_FATARROW: */
  /*   default: */
  /*     // Consume token */
  /*     top_down_next(self); */
  /*     block = vec_block_last(&self->stack); */
  /*     vec_token_push(&block->lhs, *token); */
  /*     break; */
  /*   } */
  /* } */
};
