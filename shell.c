#include "shell.h"
#include "tokenize.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum TokenType {
  STRING,
  NUMBER,
  LESS,
  GREATER,
  SEMICOLON,
  // Add more as needed.
};

struct Token {
  enum TokenType type;
  char *lexeme;
  char *literal;
  int position;
};

size_t buffer_size = 256; // Capacity of the input string buffer
char *source = NULL;

struct Token *tokens = NULL;
size_t numTokens = 0; // How many tokens we've parsed
size_t capacity = 8;  // Capacity of the token buffer

int start = 0;   // Start of the current token being scanned
int current = 0; // Current position in the input string

int isAtEnd() {
  if (source == NULL) {
    exit(EXIT_FAILURE);
  }

  return current >= strlen(source);
}

void addTokenNoLiteral(enum TokenType type) {
  if (numTokens == capacity) {
    capacity *= 2;
    tokens = (struct Token *)realloc(tokens, sizeof(struct Token) * capacity);
  }

  struct Token new_token;
  new_token.type = type;
  new_token.lexeme = strndup(source + (sizeof(char)) * start, current - start);

  new_token.literal = NULL;
  new_token.position = start;

  printf("Lexeme (at %d): %s\n", new_token.position, new_token.lexeme);

  tokens[numTokens++] = new_token;
}

void addTokenWithLiteral(enum TokenType type, char *literal) {
  addTokenNoLiteral(type);
  tokens[numTokens - 1].literal = strdup(literal);

  printf("Literal: %s\n", tokens[numTokens - 1].literal);
}

char advance() { return source[current++]; }

void scanToken() {
  char c = advance();

  switch (c) {
  case '<':
    addTokenNoLiteral(LESS);
    break;

  case '>':
    addTokenNoLiteral(GREATER);
    break;

  case ';':
    addTokenNoLiteral(SEMICOLON);
    break;

  case ' ':
  case '\r':
  case '\t':
  case '\n':
    break;

  default: // Note the colon here
    printf("Parsed (at %d): %c\n", start, c);
  }
}

void scanner(char *source) {
  tokens = (struct Token *)malloc(sizeof(struct Token) * capacity);

  while (!isAtEnd()) {
    start = current;
    scanToken();
  }

  printf("Nb. recognized tokens: %zu (", numTokens);
  for (size_t i = 0; i < numTokens; i++) {
    printf("%s", tokens[i].lexeme);
  }
  printf(")");
}

int main() {
  source = (char *)malloc(sizeof(char) * buffer_size);

  // If allocation failed, exit
  if (source == NULL) {
    exit(EXIT_FAILURE);
  }

  // Error handling for reading from stdin
  if (fgets(source, buffer_size, stdin) == NULL) {
    free(source);
    exit(EXIT_FAILURE);
  }

  printf("Received from stdin: %s", source);

  scanner(source);
}
