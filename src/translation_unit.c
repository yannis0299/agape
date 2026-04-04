#include "translation_unit.h"

#include <stdio.h>

#include "eyre.h"
#include "prelude.h"
#include "str.h"

TU_t tu_new_from_file(const char *filename) {
  FILE *fp;
  char chunk[TU_DEFAULT_CAPACITY];
  usize amount;

  TU_t self = (TU_t){
      .filename = str_from(filename),
      .contents = str_new(TU_DEFAULT_CAPACITY),
  };

  fp = fopen(filename, "r");
  if (fp == NULL)
    eyre_bail("TU: Could not open file: %s", filename);

  while ((amount = fread(chunk, sizeof(char), TU_DEFAULT_CAPACITY, fp))) {
    // Append new chunk to buffer
    str_extend(&self.contents, (u8 *)chunk, amount);
  }

  if (!feof(fp) || ferror(fp)) { // Error reading file
    eyre_bail("TU: Error while reading file: %s [%d]", filename, ferror(fp));
  }
  fclose(fp);

  return self;
}
