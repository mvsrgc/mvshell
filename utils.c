#include "shell.h"
#include "stdio.h"
#include "tokenize.h"

void printTokenDebugInfo(Token token) {
  printf("Type: %-10s Position: %-5d Literal: %-20s Lexeme: %-20s",
         tokenTypeToString(token.type), token.position,
         token.literal ? token.literal : "N/A",
         token.lexeme ? token.lexeme : "N/A");

  if (token.type == NUMBER) {
    printf(" Value: %f", token.value);
  }

  printf("\n");
}
