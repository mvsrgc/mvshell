#include "shell.h"
#include "tokenize.h"
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

/**
 * @brief Determines if we've reached the end of the
 * input string.
 *
 * @return If we've reached the end.
 */
int isAtEnd(TokenizerState *state) {
  if (state->source == NULL) {
    exit(EXIT_FAILURE);
  }

  return state->current >= state->source_length;
}

/**
 * @brief Adds a token to the tokens array.
 *
 * @param TokenType The type of token
 * @param char* The literal value of the token
 * @param double If the token is numeric, it's value
 */
void addToken(TokenizerState *state, TokenType type, char *literal,
              double value) {
  if (state->numTokens == state->capacity) {
    state->capacity *= 2;
    state->tokens =
        (struct Token *)realloc(state->tokens, sizeof(Token) * state->capacity);
  }

  Token new_token;
  new_token.type = type;
  new_token.lexeme =
      strndup(state->source + state->start, state->current - state->start);
  new_token.literal = literal ? strdup(literal) : NULL;
  new_token.value = value;
  new_token.position = state->start;

  state->tokens[state->numTokens++] = new_token;
}

/**
 * @brief Returns the character at the current position,
 * and then increment the value of current.
 *
 * @return The character at position `current`.
 */
char advance(TokenizerState *state) { return state->source[state->current++]; }

/**
 * @brief Peeks the character at the current position.
 * Does not increment the current position.
 *
 * @return The character at position `current`.
 */
char peek(TokenizerState *state) {
  if (isAtEnd(state)) {
    return '\0';
  }
  return state->source[state->current];
}

/**
 * @brief Peeks the character at the current + 1 position.
 * Does not increment the current position.
 *
 * @return The character at position `current + 1`.
 */
char peekNext(TokenizerState *state) {
  if (state->current + 1 >= state->source_length) {
    return '\0';
  }

  return state->source[state->current + 1];
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
void string(TokenizerState *state) {
  // While we have not reached the end of the string (delimited by ")
  // or the end of the source string.
  while (peek(state) != '"' && !isAtEnd(state)) {
    advance(state);
  }

  if (isAtEnd(state)) {
    printf("Error: unterminated string.");
    exit(EXIT_FAILURE);
  }

  advance(state); // Consume closing "

  // Literal value of the string does not include quotations.
  char *value = strndup(state->source + (state->start + 1),
                        state->current - state->start - 2);

  addToken(state, STRING, value, 0.0);
}

/**
 * @brief Reads a number token.
 *
 * @return Nothing, the token is added to the tokens array.
 */
void number(TokenizerState *state) {
  while (isdigit(peek(state))) {
    advance(state);
  }

  // Decimal number
  if (peek(state) == '.' && isdigit(peekNext(state))) {
    advance(state);

    while (isdigit(peek(state))) {
      advance(state);
    }
  }

  char num_substr[state->current - state->start + 1];
  strncpy(num_substr, &state->source[state->start],
          state->current - state->start);
  num_substr[state->current - state->start] = '\0';

  double value = strtod(num_substr, NULL);

  addToken(state, NUMBER, num_substr, value);
}

/**
 * @brief Reads a word token.
 * A word token differs from a string token
 * because a string token is quoted whereas a
 * word token is not quoted.
 *
 * @return Nothing, the token is added to the tokens array.
 */
void word(TokenizerState *state) {
  while (isAlphaNumeric(peek(state)) || isShellCharacter(peek(state))) {
    advance(state);
  }

  char *value =
      strndup(state->source + state->start, state->current - state->start);

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
      addToken(state, WORD, glob_value, 0.0);
      if (i > 0) {
        state->tokens[state->numTokens - 1].position =
            state->tokens[state->numTokens - 1].position + strlen(glob_value) +
            1;
      }
    }

    globfree(&glob_result);
  } else {
    addToken(state, WORD, value, 0.0);
  }

  free(value);
}

void scanToken(TokenizerState *state) {
  char c = advance(state);

  switch (c) {
  case '<':
    addToken(state, LESS, NULL, 0);
    break;

  case '>':
    addToken(state, GREATER, NULL, 0);
    break;

  case ';':
    addToken(state, SEMICOLON, NULL, 0);
    break;

  case '(':
    addToken(state, OPEN_PARENS, NULL, 0);
    break;

  case ')':
    addToken(state, CLOSE_PARENS, NULL, 0);
    break;
    break;

  case '|':
    addToken(state, PIPE, NULL, 0);
    break;

  case ' ':
  case '\r':
  case '\t':
  case '\n':
    break;

  case '"':
    string(state);
    break;

  default:
    if (isdigit(c)) {
      number(state);
    } else if (isAlphaNumeric(c) ||
               isShellCharacter(c)) { // @TODO: Accept _ in word identifier ?
      word(state);
    } else {
      printf("Error: Unrecognized character.");
      exit(EXIT_FAILURE);
    }
  }
}

const char *tokenTypeToString(TokenType type) {
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

void scanner(TokenizerState *state) {
  state->tokens = (Token *)malloc(sizeof(Token) * state->capacity);

  while (!isAtEnd(state)) {
    state->start = state->current;
    scanToken(state);
  }

  for (int i = 0; i < state->numTokens; i++) {
    Token last_token = state->tokens[i];
    printTokenDebugInfo(last_token);
  }

  printf("[%zu token(s)]\n", state->numTokens);
}

/**
 * @brief Cleanup allocated memory
 *
 */
void freeTokens(TokenizerState *state) {
  if (state->tokens == NULL) {
    return;
  }

  for (size_t i = 0; i < state->numTokens; i++) {
    free(state->tokens[i].lexeme);
    free(state->tokens[i].literal);
  }

  free(state->tokens);
  state->tokens = NULL;
  state->numTokens = 0;
}

void run_command(TokenizerState *state) {
  if (state->numTokens < 1) {
    fprintf(stderr, "No command entered.\n");
    return;
  }

  char *command = state->tokens[0].lexeme;

  char *args[state->numTokens + 1];
  for (size_t i = 0; i < state->numTokens; i++) {
    args[i] = state->tokens[i].literal;
  }

  args[state->numTokens] = NULL;

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
  char *source = NULL;
  source = (char *)malloc(buffer_size); // @TODO: Buffer resize

  // If allocation failed, exit
  if (source == NULL) {
    exit(EXIT_FAILURE);
  }

  TokenizerState tokenizerState = {.tokens = NULL,
                                   .numTokens = 0,
                                   .capacity = INITIAL_TOKEN_CAPACITY,
                                   .source = source,
                                   .current = 0,
                                   .source_length = strlen(source)};

  // Error handling for reading from stdin
  if (fgets(source, buffer_size, stdin) == NULL) {
    free(source);
    exit(EXIT_FAILURE);
  }

  tokenizerState.source_length = strlen(source);

  scanner(&tokenizerState);

  run_command(&tokenizerState);

  freeTokens(&tokenizerState);
  free(source);

  exit(EXIT_SUCCESS);
}
