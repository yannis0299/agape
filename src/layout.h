#ifndef LAYOUT_H
#define LAYOUT_H

#include <stdio.h>

#include "generics.h"
#include "prelude.h"
#include "tokenizer.h"
#include "translation_unit.h"

impl_display(usize, usize) { fprintf(fmt->stream, "%zu", *self); }

impl_generics(usize, usize);

typedef struct {
  TU_t *tu;
  tokenizer_t *tokenizer;
  slice_token_t stream;
  vec_token_t tokens;
  vec_usize_t stack;
} layout_t;

layout_t layout_new_from_tokenizer(tokenizer_t *tokenizer);

token_t *layout_peek_token(layout_t *self);

token_t *layout_next_token(layout_t *self);

void layout_enrich_stream(layout_t *self);

#endif // LAYOUT_H
