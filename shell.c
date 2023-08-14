#include "shell.h"
#include "utils.h"
#include <ctype.h>
#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFFER_SIZE 256
#define INITIAL_TOKEN_CAPACITY 8

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

int isShellCharacter(char c) {
  return c == '*' || c == '?' || c == '.' || c == '-';
}

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

  addToken(STRING, value, 0.0);
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

  addToken(NUMBER, num_substr, value);
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
  while (isAlphaNumeric(peek()) || isShellCharacter(peek())) {
    advance();
  }

  char *value = strndup(source + start, current - start);

  if (strchr(value, '*') || strchr(value, '?')) {
    glob_t glob_result;
    memset(&glob_result, 0, sizeof(glob_result));

    int return_value = glob(value, GLOB_TILDE, NULL, &glob_result);

    if (return_value != 0) {
      printf("Error while globbing\n");
      exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < glob_result.gl_pathc; i++) {
      char *glob_value = glob_result.gl_pathv[i];
      addToken(WORD, glob_value, 0.0);
    }

    globfree(&glob_result);
  } else {
    addToken(WORD, value, 0.0);
  }

  free(value);
}

void scanToken() {
  char c = advance();

  switch (c) {
  case '<':
    addToken(LESS, NULL, 0);
    break;

  case '>':
    addToken(GREATER, NULL, 0);
    break;

  case ';':
    addToken(SEMICOLON, NULL, 0);
    break;

  case '(':
    addToken(OPEN_PARENS, NULL, 0);
    break;

  case ')':
    addToken(CLOSE_PARENS, NULL, 0);
    break;
    break;

  case '|':
    addToken(PIPE, NULL, 0);
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
    } else if (isAlphaNumeric(c) ||
               isShellCharacter(c)) { // @TODO: Accept _ in word identifier ?
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

  printf("[%zu token(s)]\n", numTokens);
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

void run_command() {
  if (numTokens < 1) {
    fprintf(stderr, "No command entered.\n");
    return;
  }

  char *command = tokens[0].lexeme;

  char *args[numTokens + 1];
  for (size_t i = 0; i < numTokens; i++) {
    args[i] = tokens[i].literal;
  }

  args[numTokens] = NULL;

  pid_t pid = fork();
  if (pid == 0) {
    execvp(command, args);
    perror("exec");
    exit(EXIT_FAILURE);
  } else if (pid > 0) {
    wait(NULL);
  } else {
    perror("fork");
  }
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

  scanner(source);

  run_command();

  freeTokens();
  free(source);

  exit(EXIT_SUCCESS);
}
