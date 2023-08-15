#pragma once

#ifndef UTILS_H_
#define UTILS_H_

#include "shell.h"
#include "tokenize.h"

void printTokenDebugInfo(Token token);

int isAlphaNumeric(char c);

int isShellCharacter(char c);

#endif // UTILS_H_
