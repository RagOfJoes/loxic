#ifndef scanner_h
#define scanner_h

#include "token.h"
#include <stddef.h>

typedef struct {
  const char *p_source;
  size_t current, start;
  int line;

  // Linked list of tokens
  Token *p_head, *p_tail;
} Scanner;

Scanner *scanner_init(const char *source);
void scan_tokens(Scanner *s);

#endif // !scanner_h
