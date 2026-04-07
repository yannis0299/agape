#include "pratt.h"

#include "arena.h"
#include "eyre.h"
#include "tokenizer.h"

pratt_t pratt_new_from_tokenizer(tokenizer_t *tokenizer) {
  return (pratt_t){
      .tu = tokenizer->tu,
      .tokenizer = tokenizer,
      .pos = 0,
      .tokens = vec_token_as_slice(&tokenizer->tokens),
  };
}

token_t *pratt_peek_token(pratt_t *self) {
  if (self->pos >= self->tokens.len)
    return NULL;
  else
    return &self->tokens.ptr[self->pos];
}

token_t *pratt_next_token(pratt_t *self) {
  if (self->pos >= self->tokens.len)
    return NULL;
  else
    return &self->tokens.ptr[self->pos++];
}

token_t *pratt_expect_token(pratt_t *self, token_kind_t kind) {
  token_t *token = pratt_next_token(self);
  if (token == NULL) {
    eyre_bail("pratt_expect_token: unexpected end of token stream");
  } else {
    if (token->kind == kind) {
      return token;
    } else {
      eyre_bail("pratt_expect_token: expected %s token",
                TOKEN_PRINT_TABLE[kind]);
    }
  }
  return NULL;
}

expr_t *pratt_parse_atom(pratt_t *self) {
  expr_t *expr = NULL;
  token_t *token = NULL;
  printf("pratt_parse_atom: pos = %zu \n", self->pos);
  while ((token = pratt_peek_token(self))) {
    switch (token->kind) {
    case TOKEN_IDENTIFIER:
      // Eat identifier
      pratt_next_token(self);
      expr = ga_alloc(sizeof(expr_t));
      expr->kind = EXPR_VARIABLE;
      expr->as.var = token->repr;
      return expr;
    case TOKEN_LEFT_PAREN:
      // Eat left paren
      pratt_next_token(self);
      // Parse inner expression
      expr = pratt_parse_expr(self);
      // Expect right paren
      pratt_expect_token(self, TOKEN_RIGHT_PAREN);
      return expr;
    case TOKEN_EOL:
      // Skip for now
      pratt_next_token(self);
      continue;
    case TOKEN_RIGHT_PAREN:
    case TOKEN_EOF:
    case TOKEN_BACKSLASH:
    case TOKEN_DOT:
    case TOKEN_MAX_KIND:
      return NULL;
    }
  }
  return expr;
}

expr_t *pratt_parse_expr(pratt_t *self) {
  expr_t *lhs = NULL, *rhs = NULL;
  expr_t *expr = NULL;
  token_t *token = NULL;
  printf("pratt_parse_expr: pos = %zu \n", self->pos);
  while ((token = pratt_peek_token(self))) {
    switch (token->kind) {
    case TOKEN_IDENTIFIER:
    case TOKEN_LEFT_PAREN:
      // Parse maybe lhs of application
      lhs = pratt_parse_atom(self);
      // Try to parse the rhs of an application
      rhs = pratt_parse_atom(self);
      if (rhs == NULL)
        return lhs;
      else {
        expr = ga_alloc(sizeof(expr_t));
        expr->kind = EXPR_APPLICATION;
        expr->as.app.lhs = lhs;
        expr->as.app.rhs = rhs;
        return expr;
      }
    case TOKEN_BACKSLASH:
      // Eat backslash
      pratt_next_token(self);
      // Eat identifier
      token = pratt_expect_token(self, TOKEN_IDENTIFIER);
      // Eat dot
      pratt_expect_token(self, TOKEN_DOT);
      // Eat body
      rhs = pratt_parse_expr(self);
      expr = ga_alloc(sizeof(expr_t));
      expr->kind = EXPR_LAMBDA;
      expr->as.fn.lhs = token->repr;
      expr->as.fn.rhs = rhs;
      return expr;
    case TOKEN_EOL:
      // Skip for now
      pratt_next_token(self);
      continue;
    case TOKEN_EOF:
    case TOKEN_DOT:
    case TOKEN_RIGHT_PAREN:
    case TOKEN_MAX_KIND:
      return NULL;
    }
  }
  return expr;
}
