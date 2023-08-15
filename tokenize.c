#include "tokenize.h"

/**
 * @brief Adds a token to the tokens array.
 *
 * @param TokenType The type of token
 * @param char* The literal value of the token
 * @param double If the token is numeric, it's value
 */
void addToken(enum TokenType type, char *literal, double value) {
  if (numTokens == capacity) {
    capacity *= 2;
    tokens = (struct Token *)realloc(tokens, sizeof(struct Token) * capacity);
  }

  struct Token new_token;
  new_token.type = type;
  new_token.lexeme = strndup(source + start, current - start);
  new_token.literal = literal ? strdup(literal) : NULL;
  new_token.value = value;
  new_token.position = start;

  tokens[numTokens++] = new_token;
}
