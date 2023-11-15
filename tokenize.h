#ifndef TOKENIZE_H_
#define TOKENIZE_H_

#define BUFFER_SIZE 256
#define INITIAL_TOKEN_CAPACITY 8

#include <stddef.h>

typedef enum {
  STRING,
  NUMBER,
  LESS,
  GREATER,
  GREATERGREATER,
  SEMICOLON,
  OPEN_PARENS,
  CLOSE_PARENS,
  PIPE,
  BACKSLASH,
  WORD
  // Add more as needed.
} Lexeme;

typedef struct {
  Lexeme lexeme;
  char *literal;
  double value;
  int position;
} Token;

typedef struct {
  Token *tokens;
  size_t numTokens;
  size_t capacity;
  char *source;
  size_t current;
  size_t source_length;
  size_t start;
} TokenizerState;

int isAtEnd(TokenizerState *state);

void addToken(TokenizerState *state, Lexeme type, char *literal, double value);

char advance(TokenizerState *state);

char peek(TokenizerState *state);

char peekNext(TokenizerState *state);

void string(TokenizerState *state);

void number(TokenizerState *state);

void word(TokenizerState *state);

void scanToken(TokenizerState *state);

const char *lexemeToString(Lexeme type);

void scanner(TokenizerState *state);

TokenizerState initializeTokenizerState(size_t bufferSize);

void freeTokens(TokenizerState *state);

void destroyTokenizerState(TokenizerState *state);

#endif // TOKENIZE_H_
