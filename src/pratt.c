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
  while ((token = pratt_next_token(self))) {
    switch (token->kind) {
    case TOKEN_IDENTIFIER:
      expr = ga_alloc(sizeof(expr_t));
      expr->kind = EXPR_VARIABLE;
      expr->as.var = token->repr;
      return expr;
    case TOKEN_LEFT_PAREN:
      // Parse inner expression
      expr = pratt_parse_expr(self);
      // Expect right paren
      pratt_expect_token(self, TOKEN_RIGHT_PAREN);
      return expr;
    case TOKEN_EOL:
      continue;
    default:
      eyre_bail("pratt_parse_atom: unexpected token %s",
                TOKEN_PRINT_TABLE[token->kind]);
    }
  }
  return NULL;
}

expr_t *pratt_parse_atoms(pratt_t *self) {
  expr_t *expr = NULL, *atom = NULL;
  token_t *token = NULL;
  while ((token = pratt_peek_token(self))) {
    switch (token->kind) {
    case TOKEN_IDENTIFIER:
    case TOKEN_LEFT_PAREN:
      atom = pratt_parse_atom(self);
      if (expr == NULL) {
        expr = atom;
      } else {
        expr_t *app = (expr_t *)ga_alloc(sizeof(expr_t));
        app->kind = EXPR_APPLICATION;
        app->as.app.lhs = expr;
        app->as.app.rhs = atom;
        expr = app;
      }
      continue;
    case TOKEN_EOL:
      pratt_next_token(self);
      continue;
    default:
      return expr;
    }
  }
  return NULL;
}

expr_t *pratt_parse_lambda(pratt_t *self) {
  expr_t *expr = NULL;
  token_t *token = NULL;
  vec_token_t acc = vec_token_new(1);
  while ((token = pratt_peek_token(self))) {
    switch (token->kind) {
    case TOKEN_BACKSLASH:
      // Eat backslash
      pratt_next_token(self);
      // Eat first identifier
      token = pratt_expect_token(self, TOKEN_IDENTIFIER);
      vec_token_push(&acc, *token);
      continue;
    case TOKEN_IDENTIFIER:
      // Eat identifier
      pratt_next_token(self);
      vec_token_push(&acc, *token);
      continue;
    case TOKEN_DOT:
      if (acc.len == 0) {
        eyre_bail("pratt_parse_lambda: expected at least one identifier "
                  "between backslash and dot");
      } else {
        // Eat dot
        pratt_expect_token(self, TOKEN_DOT);
        // Eat body
        expr = pratt_parse_expr(self);
        // Reduce identifier tower to chained lambdas
        for (isize idx = acc.len - 1; idx >= 0; idx--) {
          token = &acc.raw[idx];
          expr_t *fn = (expr_t *)ga_alloc(sizeof(expr_t));
          fn->kind = EXPR_LAMBDA;
          fn->as.fn.lhs = token->repr;
          fn->as.fn.rhs = expr;
          expr = fn;
        }
        return expr;
      }
    default:
      eyre_bail("pratt_parse_lambda: unexpected token %s",
                TOKEN_PRINT_TABLE[token->kind]);
    }
  }
  return NULL;
}

expr_t *pratt_parse_expr(pratt_t *self) {
  token_t *token = NULL;
  while ((token = pratt_peek_token(self))) {
    switch (token->kind) {
    case TOKEN_IDENTIFIER:
    case TOKEN_LEFT_PAREN:
      return pratt_parse_atoms(self);
    case TOKEN_BACKSLASH:
      return pratt_parse_lambda(self);
    case TOKEN_EOL:
      // Skip for now
      pratt_next_token(self);
      continue;
    default:
      eyre_bail("pratt_parse_expr: unexpected token %s",
                TOKEN_PRINT_TABLE[token->kind]);
    }
  }
  return NULL;
}
