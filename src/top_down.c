#include "top_down.h"

top_down_t top_down_new_from_tokenizer(tokenizer_t *tokenizer) {
  vec_block_t stack = vec_block_new(1);
  return (top_down_t){
      .tu = tokenizer->tu,
      .pos = 0,
      .tokens = vec_token_as_slice(&tokenizer->tokens),
      .stack = stack,
  };
}

token_t *top_down_peek(top_down_t *self) { return NULL; }

token_t *top_down_next(top_down_t *self) { return NULL; };

void top_down_exhaust(top_down_t *self) { return; };
