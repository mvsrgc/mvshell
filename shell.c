#include "parse.h"
#include "tokenize.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

size_t buffer_size = BUFFER_SIZE; // Capacity of the input string buffer

void run_command(TokenizerState *state) {
  if (state->numTokens < 1) {
    fprintf(stderr, "No command entered.\n");
    return;
  }

  char *command = state->tokens[0].literal;

  char *args[state->numTokens + 1];
  args[state->numTokens] = NULL; // execvp expects null-terminated array
  for (size_t i = 0; i < state->numTokens; i++) {
    args[i] = state->tokens[i].literal;
  }

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
  TokenizerState tokenizerState = initializeTokenizerState(BUFFER_SIZE);

  // If allocation failed, exit
  if (tokenizerState.source == NULL) {
    exit(EXIT_FAILURE);
  }

  // Error handling for reading from stdin
  if (fgets(tokenizerState.source, buffer_size, stdin) == NULL) {
    destroyTokenizerState(&tokenizerState);
    exit(EXIT_FAILURE);
  }

  tokenizerState.source_length = strlen(tokenizerState.source);

  scanner(&tokenizerState);

  ParserState parserState =
      initializeParserState(tokenizerState.numTokens, tokenizerState.tokens);

  parse(&parserState);

  run_command(&tokenizerState);

  destroyTokenizerState(&tokenizerState);

  exit(EXIT_SUCCESS);
}
