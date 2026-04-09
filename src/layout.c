#include "layout.h"

#include "eyre.h"
#include "str.h"
#include "tokenizer.h"

layout_t layout_new_from_tokenizer(tokenizer_t *tokenizer) {
  vec_token_t tokens = vec_token_new(1);
  vec_usize_t stack = vec_usize_new(1);
  return (layout_t){
      .tu = tokenizer->tu,
      .tokenizer = tokenizer,
      .stream = vec_token_as_slice(&tokenizer->tokens),
      .pos = 0,
      .tokens = tokens,
      .stack = stack,
  };
}

token_t *layout_peek_token(layout_t *self) {
  if (self->pos >= self->stream.len)
    return NULL;
  else
    return &self->stream.ptr[self->pos];
}

token_t *layout_next_token(layout_t *self) {
  if (self->pos >= self->stream.len)
    return NULL;
  else
    return &self->stream.ptr[self->pos++];
}

usize layout_last_column(layout_t *self) {
  usize *column = vec_usize_last(&self->stack);
  if (column == NULL) {
    eyre_bail("layout_last_column: empty stack");
    return 0;
  } else {
    return *column;
  }
}

void layout_enrich_stream(layout_t *self) {
  i32 head_token = 1;
  token_t *token = NULL;
  // Skip prefixing newlines
  while ((token = layout_peek_token(self)) && token->kind == TOKEN_EOL) {
    layout_next_token(self);
  }
  if (token->column != 1)
    eyre_bail("layout_enright_stream: top-level bindings must be indented on "
              "the first column");
  // Add brace
  vec_token_push(&self->tokens, (token_t){.kind = TOKEN_LEFT_BRACE,
                                          .line = token->line,
                                          .column = token->column,
                                          .start = token->start,
                                          .span = token->span,
                                          .repr = str_from("{")});
  // Push column to context stack
  vec_usize_push(&self->stack, token->column);
  // Push to token to stream
  layout_next_token(self);
  vec_token_push(&self->tokens, *token);
  // Heavy lifting
  while ((token = layout_peek_token(self)) && token->kind != TOKEN_EOF) {
    switch (token->kind) {
    case TOKEN_WHERE:
      // Eat where
      layout_next_token(self);
      vec_token_push(&self->tokens, *token);
      // Add brace
      vec_token_push(&self->tokens, (token_t){.kind = TOKEN_LEFT_BRACE,
                                              .line = token->line,
                                              .column = token->column,
                                              .start = token->start,
                                              .span = token->span,
                                              .repr = str_from("{")});
      // Eat trailing newlines
      while ((token = layout_peek_token(self)) && token->kind == TOKEN_EOL) {
        layout_next_token(self);
      }
      // Push column to context stack
      vec_usize_push(&self->stack, token->column);
      // Push to token to stream
      layout_next_token(self);
      vec_token_push(&self->tokens, *token);
      head_token = 0;
      continue;
    case TOKEN_EOL:
      head_token = 1;
      layout_next_token(self);
      continue;
    default:
      if (head_token) {
        // Eat first token from line
        head_token = 0;
        usize column = layout_last_column(self);
        if (token->column == column) { // Push ;
          vec_token_push(&self->tokens, (token_t){.kind = TOKEN_SEMICOLON,
                                                  .line = token->line,
                                                  .column = token->column,
                                                  .start = token->start,
                                                  .span = token->span,
                                                  .repr = str_from(";")});
          continue;
        } else if (token->column > column) { // Expression continue
          layout_next_token(self);
          vec_token_push(&self->tokens, *token);
          continue;
        } else { // Close blocks until matching indentation level
          while (token->column != column) {
            vec_token_push(&self->tokens, (token_t){.kind = TOKEN_RIGHT_BRACE,
                                                    .line = token->line,
                                                    .column = token->column,
                                                    .start = token->start,
                                                    .span = token->span,
                                                    .repr = str_from("}")});
            if (self->stack.len != 0)
              self->stack.len--;
            else
              eyre_bail("layout_enrich_stream: exhausted stack before matching "
                        "closing indentation");
            column = layout_last_column(self);
          }
          head_token = 1;
          continue;
        }
      } else {
        layout_next_token(self);
        vec_token_push(&self->tokens, *token);
      }
    }
  }
  // Close all opened contexts
  while (self->stack.len != 0) {
    self->stack.len--;
    vec_token_push(&self->tokens, (token_t){.kind = TOKEN_RIGHT_BRACE,
                                            .line = token->line,
                                            .column = token->column,
                                            .start = token->start,
                                            .span = token->span,
                                            .repr = str_from("}")});
  }
}
