#include "shell.h"
#include "tokenize.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 256
#define INITIAL_TOKEN_CAPACITY 8

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

size_t buffer_size = BUFFER_SIZE; // Capacity of the input string buffer
char *source = NULL;

struct Token *tokens = NULL;
size_t numTokens = 0;                     // How many tokens we've parsed
size_t capacity = INITIAL_TOKEN_CAPACITY; // Capacity of the token buffer

int start = 0;   // Start of the current token being scanned
int current = 0; // Current position in the input string

int source_length = 0;

/**
 * @brief Determines if we've reached the end of the
 * input string.
 *
 * @return If we've reached the end.
 */
int isAtEnd() {
  if (source == NULL) {
    exit(EXIT_FAILURE);
  }

  return current >= source_length;
}

void addTokenNoLiteral(enum TokenType type) {
  if (numTokens == capacity) {
    capacity *= 2;
    tokens = (struct Token *)realloc(tokens, sizeof(struct Token) * capacity);
  }

  struct Token new_token;
  new_token.type = type;
  new_token.lexeme = strndup(source + start, current - start);

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

/**
 * @brief Returns the character at the current position,
 * and then increment the value of current.
 *
 * @return The character at position `current`.
 */
char advance() { return source[current++]; }

/**
 * @brief Peeks the character at the current position.
 * Does not increment the current position.
 *
 * @return The character at position `current`.
 */
char peek() {
  if (isAtEnd()) {
    return '\0';
  }
  return source[current];
}

/**
 * @brief Peeks the character at the current + 1 position.
 * Does not increment the current position.
 *
 * @return The character at position `current + 1`.
 */
char peekNext() {
  if (current + 1 >= source_length) {
    return '\0';
  }

  return source[current + 1];
}

int isAlphaNumeric(char c) { return isalpha(c) || isdigit(c); }

/**
 * @brief Reads a string token.
 *
 * @return Nothing, the token is added to the tokens array.
 */
void string() {
  // While we have not reached the end of the string (delimited by ")
  // or the end of the source string.
  while (peek() != '"' && !isAtEnd()) {
    advance();
  }

  if (isAtEnd()) {
    printf("Error: unterminated string.");
    exit(EXIT_FAILURE);
  }

  advance(); // Consume closing "

  // Literal value of the string does not include quotations.
  char *value = strndup(source + (start + 1), current - start - 2);

  addStringToken(STRING, value);
}

/**
 * @brief Reads a number token.
 *
 * @return Nothing, the token is added to the tokens array.
 */
void number() {
  while (isdigit(peek())) {
    advance();
  }

  // Decimal number
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

/**
 * @brief Reads a word token.
 * A word token differs from a string token
 * because a string token is quoted whereas a
 * word token is not quoted.
 *
 * @return Nothing, the token is added to the tokens array.
 */
void word() {
  while (isalpha(peek())) {
    advance();
  }

  char *value = strndup(source + start, current - start);

  addStringToken(WORD, value);
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

  default:
    if (isdigit(c)) {
      number();
    } else if (isalpha(c)) { // @TODO: Accept _ in word identifier ?
      word();
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
  case WORD:
    return "WORD";
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
    printf(" Value: %f", token.value);
  }

  printf("\n");
}

void scanner(char *source) {
  tokens = (struct Token *)malloc(sizeof(struct Token) * capacity);

  while (!isAtEnd()) {
    start = current;
    scanToken();
  }

  for (int i = 0; i < numTokens; i++) {
    struct Token last_token = tokens[i];
    printTokenDebugInfo(last_token);
  }

  printf("Nb. recognized tokens: %zu", numTokens);
}

/**
 * @brief Cleanup allocated memory
 *
 */
void freeTokens() {
  if (tokens == NULL) {
    return;
  }

  for (size_t i = 0; i < numTokens; i++) {
    free(tokens[i].lexeme);
    free(tokens[i].literal);
  }

  free(tokens);
  tokens = NULL;
  numTokens = 0;
}

int main() {
  source = (char *)malloc(buffer_size); // @TODO: Buffer resize

  // If allocation failed, exit
  if (source == NULL) {
    exit(EXIT_FAILURE);
  }

  // Error handling for reading from stdin
  if (fgets(source, buffer_size, stdin) == NULL) {
    free(source);
    exit(EXIT_FAILURE);
  }

  source_length = strlen(source);

  printf("Received from stdin: %s", source);

  scanner(source);

  freeTokens();
  free(source);

  exit(EXIT_SUCCESS);
}
