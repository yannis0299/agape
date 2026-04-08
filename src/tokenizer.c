#include "tokenizer.h"

#include <ctype.h>

#include "eyre.h"

tokenizer_t tokenizer_new_from_tu(TU_t *tu) {
  return (tokenizer_t){
      .tu = tu,
      .line = 1,
      .column = 1,
      .pos = 0,
      .tokens = vec_token_new(1),
  };
}

char tokenizer_peek_char(tokenizer_t *self) {
  if (self->pos >= self->tu->contents.len)
    return '\0';
  else
    return self->tu->contents.raw[self->pos];
}

char tokenizer_next_char(tokenizer_t *self) {
  if (self->pos >= self->tu->contents.len) {
    return '\0';
  } else {
    char c = (char)self->tu->contents.raw[self->pos++];
    if ('\n' == c) {
      self->line++;
      self->column = 1;
    } else {
      self->column++;
    }
    return c;
  }
}

usize tokenizer_single(tokenizer_t *self, char e) {
  char c = tokenizer_peek_char(self);
  if (e == c) {
    tokenizer_next_char(self);
    return 1;
  } else
    return 0;
}

/* usize tokenizer_multiple(tokenizer_t *self, const char *e) { */
/*   usize len = strlen(e); */
/*   char c; */
/*   for (usize idx = 0; idx < len; idx++) { */
/*     if ((c = tokenizer_peek_char(self)) && e[idx] == c) { */
/*       tokenizer_next_char(self); */
/*     } else { */
/*       return 0; */
/*     } */
/*   } */
/*   return len; */
/* } */

/* usize tokenizer_integer(tokenizer_t *self) { */
/*   usize span = 0; */
/*   char c; */
/*   i32 seen_digit = 0; */

/*   c = tokenizer_peek_char(self); */
/*   if (c == '-') { */
/*     tokenizer_next_char(self); */
/*     span++; */
/*   } */

/*   while ((c = tokenizer_peek_char(self)) && isdigit(c)) { */
/*     seen_digit = 1; */
/*     tokenizer_next_char(self); */
/*     span++; */
/*   } */

/*   if (span >= 2 || seen_digit) { */
/*     return span; */
/*   } else { */
/*     return 0; */
/*   } */
/* } */

#define TOKENIZER_RESERVED_GUARD(table, str)                                   \
  usize tlen = sizeof(table) / sizeof(table[0]);                               \
  for (usize idx = 0; idx < tlen; idx++) {                                     \
    usize slen = strlen(table[idx]);                                           \
    if ((slen == span) && strncmp(str, table[idx], span) == 0) {               \
      return 0;                                                                \
    }                                                                          \
  }

static const char *TOKENIZER_RESERVED_NAMES[] = {"pragma", "match", "with",
                                                 "where",  "do",    "let"};

usize tokenizer_identifier(tokenizer_t *self) {
  usize pos = self->pos;
  usize span = 0;
  char c;
#define ident_start(c) (c == '_' || isalpha(c))
#define ident_letter(c) (c == '_' || isalnum(c))
#define ident_suffix(c) (c == '\'')
  c = tokenizer_peek_char(self);
  if (ident_start(c)) {
    tokenizer_next_char(self);
    span++;
    while ((c = tokenizer_peek_char(self)) && ident_letter(c)) {
      tokenizer_next_char(self);
      span++;
    }
    while ((c = tokenizer_peek_char(self)) && ident_suffix(c)) {
      tokenizer_next_char(self);
      span++;
    }
    TOKENIZER_RESERVED_GUARD(TOKENIZER_RESERVED_NAMES,
                             ((char *)self->tu->contents.raw + pos));
    return span;
  } else {
    return 0;
  }
}

void tokenizer_skip_whitespaces_and_comments(tokenizer_t *self) {
  // Eat whitespaces prefixing characters
  char c;
  while ((c = tokenizer_peek_char(self)) && c == ' ')
    tokenizer_next_char(self);
}

/* static const char *TOKENIZER_RESERVED_OPERATORS[] = {"\\", "=>", "=", "<-"};
 */

/* static inline i32 op_letter(char c) { */
/*   // ':', '!', '#', '$', '%', '&', '*', '+', '.', '/', '<', '=', '>', '@', */
/*   // '\\', */
/*   // '^', '|', '-', '~' */
/*   return (c == ':' || c == '!' || c == '#' || c == '$' || c == '%' || */
/*           c == '&' || c == '*' || c == '+' || c == '.' || c == '/' || */
/*           c == '<' || c == '=' || c == '>' || c == '@' || c == '\\' || */
/*           c == '^' || c == '|' || c == '-' || c == '~'); */
/* } */

/* usize tokenizer_operator(tokenizer_t *self) { */
/*   usize pos = self->pos; */
/*   usize span = 0; */
/*   char c; */

/*   c = tokenizer_peek_char(self); */
/*   if (op_letter(c)) { */
/*     tokenizer_next_char(self); */
/*     span++; */
/*     while ((c = tokenizer_peek_char(self)) && op_letter(c)) { */
/*       tokenizer_next_char(self); */
/*       span++; */
/*     } */
/*     TOKENIZER_RESERVED_GUARD(TOKENIZER_RESERVED_OPERATORS, */
/*                              ((char *)self->tu->contents.raw + pos)); */
/*     return span; */
/*   } else { */
/*     return 0; */
/*   } */
/* } */

token_t *tokenizer_next(tokenizer_t *self) {
  tokenizer_skip_whitespaces_and_comments(self);

  usize line = self->line;
  usize column = self->column;
  usize pos = self->pos;
  usize span = 0;

#define TOKENIZER_MATCH(matcher, _kind)                                        \
  span = matcher;                                                              \
  if (span) {                                                                  \
    str_t repr = str_new(span + 1);                                            \
    str_extend(&repr, (u8 *)self->tu->contents.raw + pos, span);               \
    token_t token = {.kind = _kind,                                            \
                     .column = column,                                         \
                     .line = line,                                             \
                     .start = pos,                                             \
                     .span = span,                                             \
                     .repr = repr};                                            \
    vec_token_push(&self->tokens, token);                                      \
    return &self->tokens.raw[self->tokens.len - 1];                            \
  } else {                                                                     \
    self->line = line;                                                         \
    self->column = column;                                                     \
    self->pos = pos;                                                           \
  }

  // Match left pragma keyword
  /* TOKENIZER_MATCH(tokenizer_multiple(self, "{:"), TOKEN_LEFT_PRAGMA); */

  /* // Match right pragma keyword */
  /* TOKENIZER_MATCH(tokenizer_multiple(self, ":}"), TOKEN_RIGHT_PRAGMA); */

  /* // Match integer literal */
  /* TOKENIZER_MATCH(tokenizer_integer(self), TOKEN_INTEGER); */

  // Match identifier token
  TOKENIZER_MATCH(tokenizer_identifier(self), TOKEN_IDENTIFIER);

  /* // Match operator token */
  /* TOKENIZER_MATCH(tokenizer_operator(self), TOKEN_OPERATOR); */

  // Match end of line character
  TOKENIZER_MATCH(tokenizer_single(self, '\n'), TOKEN_EOL);

  // Match end of file character
  TOKENIZER_MATCH(tokenizer_single(self, '\0'), TOKEN_EOF);

  // Match backslash character
  TOKENIZER_MATCH(tokenizer_single(self, '\\'), TOKEN_BACKSLASH);

  // Match dot character
  TOKENIZER_MATCH(tokenizer_single(self, '.'), TOKEN_DOT);

  // Match left paren character
  TOKENIZER_MATCH(tokenizer_single(self, '('), TOKEN_LEFT_PAREN);

  // Match right paren character
  TOKENIZER_MATCH(tokenizer_single(self, ')'), TOKEN_RIGHT_PAREN);

  /* // Match left bracket character */
  /* TOKENIZER_MATCH(tokenizer_single(self, '['), TOKEN_LEFT_BRACKET); */

  /* // Match right bracket character */
  /* TOKENIZER_MATCH(tokenizer_single(self, ']'), TOKEN_RIGHT_BRACKET); */

  /* // Match left brace character */
  /* TOKENIZER_MATCH(tokenizer_single(self, '{'), TOKEN_LEFT_BRACE); */

  /* // Match right brace character */
  /* TOKENIZER_MATCH(tokenizer_single(self, '}'), TOKEN_RIGHT_BRACE); */

  /* // Match comma character */
  /* TOKENIZER_MATCH(tokenizer_single(self, ','), TOKEN_COMMA); */

  /* // Match right fat-arrow keyword */
  /* TOKENIZER_MATCH(tokenizer_multiple(self, "=>"), TOKEN_RIGHT_FATARROW); */

  /* // Match equal character */
  /* TOKENIZER_MATCH(tokenizer_single(self, '='), TOKEN_EQUAL); */

  /* // Match colon character */
  /* TOKENIZER_MATCH(tokenizer_single(self, ':'), TOKEN_COLON); */

  /* // Match semi-colon character */
  /* TOKENIZER_MATCH(tokenizer_single(self, ';'), TOKEN_SEMICOLON); */

  /* // Match match keyword token */
  /* TOKENIZER_MATCH(tokenizer_multiple(self, "match"), TOKEN_MATCH); */

  /* // Match with keyword token */
  /* TOKENIZER_MATCH(tokenizer_multiple(self, "with"), TOKEN_WITH); */

  /* // Match where keyword token */
  /* TOKENIZER_MATCH(tokenizer_multiple(self, "where"), TOKEN_WHERE); */

  /* // Match do keyword token */
  /* TOKENIZER_MATCH(tokenizer_multiple(self, "do"), TOKEN_DO); */

  /* // Match let keyword token */
  /* TOKENIZER_MATCH(tokenizer_multiple(self, "let"), TOKEN_LET); */

  /* // Match left-arrow keyword token */
  /* TOKENIZER_MATCH(tokenizer_multiple(self, "<-"), TOKEN_LEFT_ARROW); */

  // If nothing is matched
  eyre_bail("tokenizer_next: could not match any token at %zu:%zu", line,
            column);

  return NULL;
}

void tokenizer_exhaust(tokenizer_t *self) {
  token_t *token = NULL;
  do {
    token = tokenizer_next(self);
  } while (token != NULL && token->kind != TOKEN_EOF);
}
