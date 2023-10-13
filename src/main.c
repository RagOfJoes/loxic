#include "scanner.h"
#include "token.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Runs our interpreter using the given string as source code
void run(const char *source) {
  Scanner *s = scanner_init(source);
  scan_tokens(s);

  Token *tmp = NULL;
  while (s->p_head != NULL) {
    if (s->p_head->p_literal != NULL) {
      if (s->p_head->type == TOKEN_NUMBER) {
        printf("[%d]: %g\n", s->p_head->type, atof(s->p_head->p_literal));
      } else if (s->p_head->p_literal != NULL) {
        printf("[%d]: %s\n", s->p_head->type, s->p_head->p_literal);
      }

      free((char *)s->p_head->p_literal);
    } else {
      char value[s->p_head->size];
      for (int i = 0; i < s->p_head->size; i++) {
        value[i] = s->p_source[s->p_head->start + i];
      }

      printf("[%d]: %s\n", s->p_head->type, value);
    }

    tmp = s->p_head;
    s->p_head = s->p_head->p_next;
    free(tmp);
  }

  // Free memory
  free(s);

  return;
}

// Reads a file and runs our interpreter on it
void run_file(const char *path) {
  char *buffer;

  size_t file_size;
  FILE *file = fopen(path, "rb");

  if (file == NULL) {
    fprintf(stderr, "Could not open file \"%s\".\n", path);
    fclose(file);
    return;
  }

  // Seek the last byte of the file
  fseek(file, 0, SEEK_END);
  // Offset from the first to the last byte, or in other words, size of the file
  file_size = ftell(file);

  // Go back to the start of the file
  rewind(file);

  // Allocate a string that can hold the file
  buffer = (char *)malloc(sizeof(char) * (file_size + 1));
  if (!buffer) {
    fprintf(stderr, "Could not allocate buffer.\n");
    fclose(file);
    return;
  }

  // Read it all in one operation
  if (fread(buffer, sizeof(char), file_size, file) != file_size) {
    fprintf(stderr, "Could not read file \"%s\".\n", path);
    fclose(file);
    return;
  }

  // Mark EOF as '\0'
  buffer[file_size] = '\0';

  // Run interpreter
  run(buffer);

  // Free memory and close file
  fclose(file);
  free(buffer);

  return;
}

// Runs our interpreter in a REPL
void run_prompt() {
  char line[1024];
  size_t line_number = 1;

  // Prompt user to enter a line of code
  printf("Welcome to Lox\'s REPL!\n");
  for (;;) {
    printf("%ld> ", line_number++);

    // Read line of code
    if (!fgets(line, 1024, stdin)) {
      printf("\n");
      break;
    }

    run(line);
  }

  return;
}

int main(int argc, char **argv) {
  if (argc > 2) {
    fprintf(stderr, "Usage: loxic [script]\n");
  } else if (argc == 2) {
    run_file(argv[1]);
  } else {
    run_prompt();
  }

  return 0;
}
