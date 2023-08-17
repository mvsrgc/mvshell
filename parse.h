//
// Created by user on 8/15/23.
//

#ifndef MVSHELL_PARSE_H
#define MVSHELL_PARSE_H

#include <glob.h>
#include "tokenize.h"

typedef struct Command Command;
typedef struct ParserState ParserState;

struct Command {
    char* cmd;
    char** args;
    size_t numArgs;
    Command* next; // For piped commands
    char* outFile;
    char* inFile;
    char* appendFile;
    char* errFile;
};

struct ParserState {
    Token *tokens;
    size_t numTokens;
    size_t currentPosition;
};

void parse(ParserState *parserState);

int match(ParserState *state, Lexeme lexeme);

Token parseString(ParserState *state);
Token parseNumber(ParserState *state);
Token parseLess(ParserState *state);
Token parseGreater(ParserState *state);
Token parseOpenParens(ParserState *state);
Token parseCloseParens(ParserState *state);
Token parsePipe(ParserState *state);
Token parseWord(ParserState *state);

ParserState initializeParserState(size_t numTokens, Token *tokens);

#endif //MVSHELL_PARSE_H
