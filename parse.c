//
// Created by user on 8/15/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"
#include "tokenize.h"

Token* peekPrevious(ParserState *state) {
    Token *previousToken = NULL;
    if (state->currentPosition > 0) {
        previousToken = &state->tokens[state->currentPosition - 1];
    }

    return previousToken;
}

ParserState initializeParserState(size_t numTokens, Token *tokens) {
    ParserState state = {
            .currentPosition = 0,
            .numTokens = numTokens,
            .tokens = tokens
    };

    return state;
}

void advanceParser(ParserState *state) {
    if (state->currentPosition < state->numTokens) {
        state->currentPosition++;
    }
}

void parseStatement(ParserState *state) {
    if (match(state, STRING)) {
        parseString(state);
    } else if (match(state, NUMBER)) {
        parseNumber(state);
    } else if (match(state, WORD)) {
        parseWord(state);
    } else {
        printf("Error: Unexpected token.\n");
        exit(EXIT_FAILURE);
    }
}

void parse(ParserState *state) {
    while (state->currentPosition < state->numTokens) {
        parseStatement(state);
    }
}

int match(ParserState *state, Lexeme lexeme) {
    if (state->currentPosition == state->numTokens) return 0;
    return state->tokens[state->currentPosition].lexeme == lexeme;
}

Token parseString(ParserState *state) {
    if (match(state, STRING)) {
        Token token = state->tokens[state->currentPosition];
        advanceParser(state);
        return token;
    }

    printf("Error: Expected string literal.\n");
    exit(EXIT_FAILURE);
}

Token parseNumber(ParserState *state) {
    if (match(state, NUMBER)) {
        Token token = state->tokens[state->currentPosition];
        advanceParser(state);
        return token;
    }

    printf("Error: Expected number.\n");
    exit(EXIT_FAILURE);
}

// @TODO:
// ls -l works, ls * -l works, but ls "test.txt" -l does not work
Token parseWord(ParserState *state) {
    Token *previousToken = peekPrevious(state);

    if (match(state, WORD)) {
        Token *token = &state->tokens[state->currentPosition];

        if (token->literal != NULL && token->literal[0] == '-') {
            if (previousToken == NULL || previousToken->lexeme != WORD) {
                printf("Error: Argument list must be preceded by a command.\n");
                exit(EXIT_FAILURE);
            } else {
                advanceParser(state);
                return *token;
            }
        } else {
            advanceParser(state);
            return *token;
        }
    }

    printf("Error: Expected command or argument.\n");
    exit(EXIT_FAILURE);
}
