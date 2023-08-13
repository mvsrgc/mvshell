#include "shell.h"
#include "tokenize.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum TokenType {
  STRING,
  NUMBER,
  LESS,
  GREATER,
  SEMICOLON,
  OPEN_PARENS,
  CLOSE_PARENS,
  PIPE,
  WORD
  // Add more as needed.
};

struct Token {
  enum TokenType type;
  char *lexeme;
  char *literal;
  double value;
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
  new_token.value = 0.0;
  new_token.position = start;

  tokens[numTokens++] = new_token;
}

void addNumberToken(enum TokenType type, double value) {
  addTokenNoLiteral(type);
  tokens[numTokens - 1].value = value;
}

void addStringToken(enum TokenType type, char *literal) {
  addTokenNoLiteral(type);
  tokens[numTokens - 1].literal = strdup(literal);
}

void addTokenWithLiteral(enum TokenType type, char *literal, double value) {
  addTokenNoLiteral(type);
  tokens[numTokens - 1].literal = strdup(literal);
  tokens[numTokens - 1].value = value;
}

char advance() { return source[current++]; }

char peek() {
  if (isAtEnd()) {
    return '\0';
  }
  return source[current];
}

char peekNext() {
  if (current + 1 >= strlen(source)) {
    return '\0';
  }

  return source[current + 1];
}

int isAlphaNumeric(char c) { return isalpha(c) || isdigit(c); }

void string() {
  while (peek() != '"' && !isAtEnd()) {
    advance();
  }

  if (isAtEnd()) {
    printf("Error: unterminated string.");
    exit(EXIT_FAILURE);
  }

  advance(); // Closing "

  char *value =
      strndup(source + sizeof(char) * (start + 1), current - start - 2);

  addStringToken(STRING, value);
}

void number() {
  while (isdigit(peek())) {
    advance();
  }

  if (peek() == '.' && isdigit(peekNext())) {
    advance();

    while (isdigit(peek())) {
      advance();
    }
  }

  char num_substr[current - start + 1];
  strncpy(num_substr, &source[start], current - start);
  num_substr[current - start] = '\0';

  double value = strtod(num_substr, NULL);

  addNumberToken(NUMBER, value);
}

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

  case '(':
    addTokenNoLiteral(OPEN_PARENS);
    break;

  case ')':
    addTokenNoLiteral(CLOSE_PARENS);
    break;

  case '|':
    addTokenNoLiteral(PIPE);
    break;

  case ' ':
  case '\r':
  case '\t':
  case '\n':
    break;

  case '"':
    string();
    break;

  default: // Note the colon here
    if (isdigit(c)) {
      number();
    } else if (isalpha(c)) { // Accept _ ?
      // word();
    } else {
      printf("Error: Unrecognized character.");
      exit(EXIT_FAILURE);
    }
  }
}

const char *tokenTypeToString(enum TokenType type) {
  switch (type) {
  case NUMBER:
    return "NUMBER";
  case STRING:
    return "STRING";
  case PIPE:
    return "PIPE";
  case LESS:
    return "<";
  case GREATER:
    return ">";
  case OPEN_PARENS:
    return "(";
  case CLOSE_PARENS:
    return ")";
  case SEMICOLON:
    return ";";
  // Add cases for other token types...
  default:
    return "UNKNOWN";
  }
}

void printTokenDebugInfo(struct Token token) {
  printf("Type: %-10s Position: %-5d Literal: %-20s Lexeme: %-20s",
         tokenTypeToString(token.type), token.position,
         token.literal ? token.literal : "N/A",
         token.lexeme ? token.lexeme : "N/A");

  if (token.type == NUMBER) {
    printf(" Value: %f", token.value); // Removed the comma
  }

  printf("\n");
}

void scanner(char *source) {
  tokens = (struct Token *)malloc(sizeof(struct Token) * capacity);

  while (!isAtEnd()) {
    start = current;
    scanToken();

    struct Token last_token = tokens[numTokens - 1];
    printTokenDebugInfo(last_token);
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

  free(source);
  free(tokens);

  exit(EXIT_SUCCESS);
}
