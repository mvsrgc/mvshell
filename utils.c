#include "shell.h"
#include "stdio.h"
#include "tokenize.h"
#include <ctype.h>

int isAlphaNumeric(char c) { return isalpha(c) || isdigit(c); }

int isShellCharacter(char c) {
    return c == '*' || c == '?' || c == '.' || c == '-';
}

void printTokenDebugInfo(Token token) {
    printf("Lexeme: %-10s Position: %-5d Literal: %-10s",
           lexemeToString(token.lexeme), token.position,
           token.literal ? token.literal : "N/A");

    if (token.lexeme == NUMBER) {
        printf(" Value: %f", token.value);
    }

    printf("\n");
}
