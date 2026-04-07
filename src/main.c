#include <stdio.h>

#include "arena.h"
#include "eyre.h"
#include "generics.h"
#include "pratt.h"
#include "prelude.h"
#include "str.h"
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

  printf("--- Expr: ---------------------\n");
  pratt_t pratt = pratt_new_from_tokenizer(&tokenizer);
  expr_t *expr = pratt_parse_expr(&pratt);
  println(expr, expr);

  /* expr_t x = EXPR_VAR("x"); */
  /* expr_t y = EXPR_VAR("y"); */
  /* expr_t z = EXPR_VAR("z"); */
  /* expr_t id = EXPR_FN("x", &x); */
  /* expr_t yz = EXPR_APP(&y, &z); */
  /* expr_t e = EXPR_APP(&id, &yz); */
  /* println(expr, &e); */

  /* printf("--- Blocks: -------------------\n"); */
  /* top_down_t top_down = top_down_new_from_tokenizer(&tokenizer); */
  /* top_down_exhaust(&top_down); */
  /* println(vec_block, &top_down.stack); */

  ga_destroy();
  return 0;
}
