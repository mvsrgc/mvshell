#ifndef TOKENIZE_H_
#define TOKENIZE_H_

#include <stddef.h>
typedef enum {
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
} TokenType;

typedef struct {
  TokenType type;
  char *lexeme;
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

const char *tokenTypeToString(TokenType type);

void addToken(TokenizerState *state, TokenType type, char *literal,
              double value);

#endif // TOKENIZE_H_
