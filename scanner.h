#ifndef SCANNER_H
#define SCANNER_H

#include "token.h"

Token scanner();
extern int lineNumber;
extern char nextChar;
extern std::istream *input_stream;

#endif