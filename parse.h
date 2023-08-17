//
// Created by user on 8/15/23.
//

#ifndef MVSHELL_PARSE_H
#define MVSHELL_PARSE_H

#include <glob.h>

typedef struct Command Command;

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

#endif //MVSHELL_PARSE_H
