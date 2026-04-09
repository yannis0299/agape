#include <stdio.h>

#include "arena.h"
#include "generics.h"
#include "layout.h"
#include "pratt.h"
#include "prelude.h"
#include "str.h"
#include "tokenizer.h"
#include "translation_unit.h"

void do_file(const char *filename) {
  TU_t tu = tu_new_from_file(filename);
  printf("--- File: %10s ----------\n", tu.filename.raw);
  printf("%s", tu.contents.raw);

  printf("--- Tokens: -------------------\n");
  tokenizer_t tokenizer = tokenizer_new_from_tu(&tu);
  tokenizer_exhaust(&tokenizer);
  println(vec_token, &(tokenizer.tokens));

  printf("--- Layout: -------------------\n");
  layout_t layout = layout_new_from_tokenizer(&tokenizer);
  layout_enrich_stream(&layout);
  println(vec_token, &(layout.tokens));

  printf("--- Expr: ---------------------\n");
  pratt_t pratt = pratt_new_from_tokenizer(&tokenizer);
  expr_t *expr = pratt_parse_expr(&pratt);
  println(expr, expr);
}

i32 main(i32 argc, char *argv[]) {
  ga_init(128 * 1024 * 1024); // 128 MiB

  for (i32 idx = 1; idx < argc; idx++) {
    do_file(argv[idx]);
  }

  ga_destroy();
  return 0;
}
