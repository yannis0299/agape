#include <stdio.h>

#include "arena.h"
#include "generics.h"
#include "prelude.h"
#include "tokenizer.h"
#include "translation_unit.h"

impl_generics(u8, u8);

i32 main(i32 argc, char *argv[]) {
  ga_init(128 * 1024 * 1024); // 128 MiB

  TU_t tu = tu_new_from_file("foo.ln");
  printf("--- File: %10s ----------\n", tu.filename.raw);
  printf("%s", tu.contents.raw);

  printf("--- Tokens: -------------------\n");
  tokenizer_t tokenizer = tokenizer_new_from_tu(&tu);
  tokenizer_exhaust(&tokenizer);
  token_t *token = NULL;
  for (usize idx = 0; idx < tokenizer.tokens.len; idx++) {
    token = &tokenizer.tokens.raw[idx];
    printf("Token { .kind = %s, .line = %zu, .column = %zu, .start = %zu, "
           ".span = %zu, .repr = \"%s\" }\n",
           TOKEN_PRINT_TABLE[token->kind], token->line, token->column,
           token->start, token->span,
           (token->kind != TOKEN_EOL) ? ((char *)token->repr.raw) : "\\n");
  }

  ga_destroy();
  return 0;
}
