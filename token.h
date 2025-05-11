#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum tokenID {
    t1_tk,     // Token type t1
    t2_tk,     // Token type t2
    t3_tk,     // Token type t3
    EOFTk,     // End of File token
    ERROR_TK,  // Error token
    NULL_TK,   // Null token
    EMTPY_TK,  // Empty token
};

struct Token {
    tokenID id;            // Token type
    std::string instance;  // Token instance (lexeme)
    int lineNumber;        // Line number where the token starts
};

std::string tokenToString(tokenID id);  // Function declaration

#endif