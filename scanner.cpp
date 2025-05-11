#include "scanner.h"

#include <fstream>
#include <iostream>
#include <string>

// #include "testScanner.h"
#include "token.h"

// Driver Table
const int driverTable[6][6] = {
    /*EOF, LETTER, DIGIT, PLUS, SC, WS*/
    /* state 0 */ {1000, 1, -1, 3, 5, 0},
    /* state 1 */ {-2, -2, 2, -2, -2, -2},
    /* state 2 */ {1003, 1003, 2, 1003, 1003, 1003},
    /* state 3 */ {-3, -3, 4, -3, -3, -3},
    /* state 4 */ {1002, 1002, 4, 1002, 1002, 1002},
    /* state 5 */ {1001, 1001, 1001, 1001, 1001, 1001}};

/**
 * @brief gets next character
 * @return next character in stream
 */
char getNextChar() {
    char c = input_stream->get();
    return c;
}

/**
 * @brief Checks if the provided character is whitespace
 * @param c The character to check.
 * @return true if the character matches, false otherwise.
 */
bool isWhitespace(char c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

/**
 * @brief Checks if the provided character is a special character defined in the
 * language: ! “ # $ % & ‘ ( )
 * @param c The character to check.
 * @return true if the character matches, false otherwise.
 */
bool isDefinedSpecialCharacter(char c) { return (c >= 33 && c <= 41); }

/**
 * @brief Checks if the provided character is a letter (a-z | A-Z)
 * @param c The character to check.
 * @return true if the character matches, false otherwise.
 */
bool isLetter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

/**
 * @brief Checks if the provided character is a digit (0-9)
 * @param c The character to check.
 * @return true if the character matches, false otherwise.
 */
bool isDigit(char c) { return (c >= '0' && c <= '9'); }

/**
 * @brief Skips whitespace
 */
void skipWhitespace() {
    while (isWhitespace(nextChar)) {
        // if new line when skipping whitespace add to line number
        if (nextChar == '\n') {
            lineNumber++;
        }
        nextChar = getNextChar();
    }
}

/**
 * @brief Skips comments
 */
void skipComments() {
    // loops until it finds another * character then start processing normally
    while (nextChar == '*') {
        nextChar = getNextChar();
        bool commentEndFound = false;
        while (!input_stream->eof()) {
            if (nextChar == '*') {
                commentEndFound = true;
                nextChar = getNextChar();
                break;
            }
            if (nextChar == '\n') {
                lineNumber++;
            }
            nextChar = getNextChar();
        }
        if (!commentEndFound) {
            return;
        }
        // Skip any trailing whitespace after the comment
        skipWhitespace();
    }
}

/**
 * @brief gets the column index based on character type
 * @param c The character used to match to driver table.
 * @return driver table index associated to character
 */
static int getColumn(char c) {
    if (isLetter(c)) {
        return 1;
    } else if (isDigit(c)) {
        return 2;
    } else if (c == '+') {
        return 3;
    } else if (isDefinedSpecialCharacter(c)) {
        return 4;
    } else if (isWhitespace(c)) {
        return 5;
    } else {
        return 0;
    }
}

/**
 * @brief return if state is final [final state]
 * @param state state to check if in final state
 * @return true if in final state, false if not
 */
bool isStateFinal(int state) { return (state >= 1000 || state < 0); }

/**
 * @brief validates input based on valid characters
 * @param char character that is being validated
 * @return true if valid, false if not valid
 */
bool validateInput(char c) {
    bool charValid = false;

    // Check if whitespace
    if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
        charValid = true;
    }

    // Check if A - Z
    if (c >= 65 && c <= 90) {
        charValid = true;
    }

    // Check if a - z
    if (c >= 97 && c <= 122) {
        charValid = true;
    }

    // Check if 0 - 9
    if (c >= 48 && c <= 57) {
        charValid = true;
    }

    // Check if special character ! " # $ % & ( ) * +
    if (c >= 33 && c <= 43) {
        charValid = true;
    }

    if (!charValid) {
        return false;
    }
    return charValid;
}

/**
 * @brief handle final state token
 * @param nextState final state being evaluated
 * @param S token content
 * @param lineNum line number of token
 * @return Token that was scanned in
 */
Token handleToken(int nextState, std::string S, int lineNum) {
    tokenID id;
    switch (nextState) {
        case 1000:
            id = EOFTk;
            break;
        case 1001:
            id = t1_tk;
            break;
        case 1002:
            id = t2_tk;
            break;
        case 1003:
            id = t3_tk;
            break;
        default:
            std::cerr << "SCANNER ERROR: Unknown final state " << nextState
                      << "\n";
            exit(1);
    }
    return {id, S, lineNum};
}

/**
 * @brief process string by using FSA logic
 * @return Token that belongs to FSA
 */
Token scanner() {
    int state = 0;
    int nextState;
    std::string S = "";
    int holdLineNumber = lineNumber;

    // while not in final state
    while (!isStateFinal(state)) {
        // If in 0 state then ignore whitespace
        if (state == 0) {
            skipWhitespace();
        }
        // skip comments
        skipComments();

        // handle valid character, Edge case for EOF
        // When input is invalid and not EOF then exit
        const int EOF_INT = -1;
        if (!validateInput(nextChar) && nextChar != EOF_INT) {
            std::cerr << "SCANNER ERROR: INVALID CHARACTER " << nextChar << " "
                      << lineNumber << "\n";
            exit(1);
        }

        // get column based on char value
        int column = getColumn(nextChar);
        // find next state in driver table
        nextState = driverTable[state][column];
        // negative state
        if (nextState < 0) {
            return {ERROR_TK, "SCANNER ERROR: Incomplete Token " + S,
                    lineNumber};
        }
        if (nextState >= 1000) {
            // final state
            return handleToken(nextState, S, holdLineNumber);
        } else {
            // building token
            state = nextState;
            S += nextChar;
            // holds token line number so it doesn't changed when comments or
            // whitespace is being processed
            holdLineNumber = lineNumber;
            // get next token [look ahead]
            nextChar = getNextChar();
        }
    }
    return {ERROR_TK, "SCANNER ERROR: Unexpected termination", lineNumber};
}
