#ifndef SHELL_H_
#define SHELL_H_

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

const char *tokenTypeToString(enum TokenType type);

#endif // SHELL_H_
