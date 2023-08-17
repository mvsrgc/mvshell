//
// Created by user on 8/15/23.
//

#include "parse.h"
#include "tokenize.h"

int current_token = 0;

char *parseName(TokenizerState *state) {
    if (current_token < state->numTokens) {
        Token *token = &state->tokens[current_token];
    }
}
