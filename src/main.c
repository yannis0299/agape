#include <stdio.h>

#include "arena.h"
#include "generics.h"
#include "prelude.h"
#include "tokenizer.h"
#include "top_down.h"
#include "translation_unit.h"

i32 main(i32 argc, char *argv[]) {
  ga_init(128 * 1024 * 1024); // 128 MiB

  TU_t tu = tu_new_from_file("foo.ln");
  printf("--- File: %10s ----------\n", tu.filename.raw);
  printf("%s", tu.contents.raw);

  printf("--- Tokens: -------------------\n");
  tokenizer_t tokenizer = tokenizer_new_from_tu(&tu);
  tokenizer_exhaust(&tokenizer);
  println(vec_token, &(tokenizer.tokens));

  printf("--- Blocks: -------------------\n");
  top_down_t top_down = top_down_new_from_tokenizer(&tokenizer);
  println(vec_block, &top_down.stack);

  ga_destroy();
  return 0;
}
