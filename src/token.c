#include "token.h"
#include <stdlib.h>

// Creates a new Token
Token *make_token(Token_Type type, int start, int size, int line,
                  const char *p_literal) {
  Token *token = malloc(sizeof(Token));
  token->type = type;
  token->start = start;
  token->size = size;
  token->line = line;
  token->p_literal = p_literal;

  return token;
}
