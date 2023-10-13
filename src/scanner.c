#include "scanner.h"
#include "debug.h"
#include "token.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Initialize a scanner
Scanner *scanner_init(const char *source) {
  Scanner *scanner = malloc(sizeof(Scanner));
  scanner->p_source = source;
  scanner->start = 0;
  scanner->current = 0;
  scanner->line = 1;

  scanner->p_head = NULL;
  scanner->p_tail = NULL;

  return scanner;
}

// Check if the current character is an alphabet
bool is_alpha(char c) {
  return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_'));
}

// Check if we've reached the end of the source code
bool is_at_end(Scanner *s) { return s->p_source[s->current] == '\0'; }

// Check if the current character is a digit
bool is_digit(char c) { return c >= '0' && c <= '9'; }

// Check if the current character is an alphanumeric
bool is_alphanumeric(char c) { return is_alpha(c) || is_digit(c); }

// Check if the current character matches the expected character. If it does,
// advance the scanner forward one character.
bool match(Scanner *s, char expected) {
  if (is_at_end(s)) {
    return false;
  }
  if (s->p_source[s->current] != expected) {
    return false;
  }

  s->current++;
  return true;
}

// Move the scanner forward one character
char advance(Scanner *s) {
  s->current++;
  return s->p_source[s->current - 1];
}

// Peek at the current character without advancing the scanner
char peek(Scanner *s) {
  if (is_at_end(s)) {
    return '\0';
  }

  return s->p_source[s->current];
}

// Peek at the next character without advancing the scanner
char peek_next(Scanner *s) {
  char p_next = s->p_source[s->current + 1];
  if (p_next == '\0') {
    return '\0';
  }

  return p_next;
}

// Retrieves the string at the scanner's start and current position
char *get_literal(Scanner *s) {
  char *literal = malloc(s->current - s->start);
  for (size_t i = 0; i < s->current - s->start; i++) {
    literal[i] = s->p_source[s->start + i];
  }

  return literal;
}

// Checks value to see if it's a keyword. If not, then it'll be marked
// as an identifier
Token_Type get_keyword(const char *value) {
  if (strcmp(value, "and") == 0) {
    return TOKEN_AND;
  } else if (strcmp(value, "class") == 0) {
    return TOKEN_CLASS;
  } else if (strcmp(value, "else") == 0) {
    return TOKEN_ELSE;
  } else if (strcmp(value, "false") == 0) {
    return TOKEN_FALSE;
  } else if (strcmp(value, "for") == 0) {
    return TOKEN_FOR;
  } else if (strcmp(value, "fun") == 0) {
    return TOKEN_FUN;
  } else if (strcmp(value, "if") == 0) {
    return TOKEN_IF;
  } else if (strcmp(value, "nil") == 0) {
    return TOKEN_NIL;
  } else if (strcmp(value, "or") == 0) {
    return TOKEN_OR;
  } else if (strcmp(value, "print") == 0) {
    return TOKEN_PRINT;
  } else if (strcmp(value, "return") == 0) {
    return TOKEN_RETURN;
  } else if (strcmp(value, "super") == 0) {
    return TOKEN_SUPER;
  } else if (strcmp(value, "this") == 0) {
    return TOKEN_THIS;
  } else if (strcmp(value, "true") == 0) {
    return TOKEN_TRUE;
  } else if (strcmp(value, "var") == 0) {
    return TOKEN_VAR;
  } else if (strcmp(value, "while") == 0) {
    return TOKEN_WHILE;
  }

  return TOKEN_IDENTIFIER;
}

// Adds a token to the linked list
void add_token(Scanner *s, Token_Type type, const char *p_literal) {
  Token *token =
      make_token(type, s->start, s->current - s->start, s->line, p_literal);

  if (s->p_head == NULL) {
    s->p_head = token;
    s->p_tail = token;
  } else {
    s->p_tail->p_next = token;
    s->p_tail = token;
  }

  return;
}

// Helper function for identifiers
void identifier(Scanner *s) {
  // Advance till the end of the identifier
  while (is_alphanumeric(peek(s))) {
    advance(s);
  }

  char *literal = get_literal(s);
  add_token(s, get_keyword(literal), literal);
}

// Helper function for strings
void string(Scanner *s) {
  // Advance till the end of the string or till the end of the file
  //
  // NOTE: Lox does support multiline strings
  while (peek(s) != '"' && !is_at_end(s)) {
    if (peek(s) == '\n') {
      s->line++;
    }

    advance(s);
  }

  // Handle unterminated string
  if (is_at_end(s)) {
    error(s->line, "Unterminated string.");
    return;
  }

  // Move past the closing "
  advance(s);

  add_token(s, TOKEN_STRING, get_literal(s));
}

// Helper function for numbers
void number(Scanner *s) {
  while (is_digit(peek(s))) {
    advance(s);
  }

  // Look for a fraction
  if (peek(s) == '.' && is_digit(peek_next(s))) {
    // Consume the .
    advance(s);

    while (is_digit(peek(s))) {
      advance(s);
    }
  }

  add_token(s, TOKEN_NUMBER, get_literal(s));
}

void scan_token(Scanner *s) {
  char c = advance(s);
  switch (c) {
  case '(':
    add_token(s, TOKEN_LEFT_PAREN, NULL);
    break;
  case ')':
    add_token(s, TOKEN_RIGHT_PAREN, NULL);
    break;
  case '{':
    add_token(s, TOKEN_LEFT_BRACE, NULL);
    break;
  case '}':
    add_token(s, TOKEN_RIGHT_BRACE, NULL);
    break;
  case ',':
    add_token(s, TOKEN_COMMA, NULL);
    break;
  case '.':
    add_token(s, TOKEN_DOT, NULL);
    break;
  case '-':
    add_token(s, TOKEN_MINUS, NULL);
    break;
  case '+':
    add_token(s, TOKEN_PLUS, NULL);
    break;
  case ';':
    add_token(s, TOKEN_SEMICOLON, NULL);
    break;
  case '*':
    add_token(s, TOKEN_STAR, NULL);
    break;
  case '!':
    add_token(s, match(s, '=') ? TOKEN_BANG_EQUAL : TOKEN_BANG, NULL);
    break;
  case '=':
    add_token(s, match(s, '=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL, NULL);
    break;
  case '<':
    add_token(s, match(s, '=') ? TOKEN_LESS_EQUAL : TOKEN_LESS, NULL);
    break;
  case '>':
    add_token(s, match(s, '=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER, NULL);
    break;
  case '/':
    // Advance till the end of the comment
    if (match(s, '/')) {
      while (peek(s) != '\n' && !is_at_end(s)) {
        advance(s);
      }
    } else {
      add_token(s, TOKEN_SLASH, NULL);
    }
    break;

  case ' ':
  case '\r':
  case '\t':
    // Ignore whitespaces
    break;

  case '\n':
    // Increment line number
    s->line++;
    break;

  case '"':
    string(s);
    break;

  case 'o':
    if (match(s, 'r')) {
      add_token(s, TOKEN_OR, NULL);
    }
    break;

  default:
    if (is_digit(c)) {
      number(s);
    } else if (is_alpha(c)) {
      identifier(s);
    } else {
      error(s->line, "Unexpected character.");
    }
    break;
  }
}

void scan_tokens(Scanner *s) {
  while (!is_at_end(s)) {
    s->start = s->current;
    scan_token(s);
  }

  // Append an EOF token at the end
  add_token(s, TOKEN_EOF, NULL);

  return;
}
