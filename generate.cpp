#include "generate.h"

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>

// Initialize global variables
int tempCount = 0;
int labelCount = 0;
std::vector<std::string> tempVars;
std::unordered_map<std::string, std::string> symbolTable;
std::ostream* outStream = &std::cout;

// Helper function for error handling
inline void fatal(const char* string) {
    fprintf(stderr, "\nFatal: %s\n\n", string);
    exit(1);
}

std::string createTemp() {
    std::string temp = "T" + std::to_string(tempCount++);
    tempVars.push_back(temp);
    return temp;
}

std::string newLabel() { return "L" + std::to_string(labelCount++); }

void emit(const std::string& code) { (*outStream) << code << "\n"; }

void emitLabel(const std::string& label) { emit(label + ":"); }

std::string processNumber(const std::string& num) {
    // Check if first character is a letter
    if (num.empty() || (num[0] < 65 || num[0] > 122) ||
        (num[0] > 90 && num[0] < 97)) {
        fatal("Expected a letter for the first character of a t3 token.");
    }

    // Determine if positive or negative based on case
    // Capital letter (A-Z) = positive, lowercase letter (a-z) = negative
    int caseVal = (num[0] >= 65 && num[0] <= 90) ? 1 : -1;

    // Convert to integer, ignoring first character
    std::string value = num.substr(1);
    int result = std::stoi(value) * caseVal;

    return std::to_string(result);
}

std::string processIdentifier(const std::string& id) {
    // Check if starts with +
    if (id.empty() || id[0] != '+') {
        fatal("Expected a plus sign for the first character of a t2 token.");
    }

    // Convert + to p
    std::string processed = "p" + id.substr(1);

    // If identifier not in symbol table, add it
    if (symbolTable.find(processed) == symbolTable.end()) {
        symbolTable[processed] = processed;
    }
    return processed;
}

/* OR1 */
void generateCode(node_t* node) {
    if (!node) return;

    if (node->label == "S") {
        // Process A ( B B )
        generateCode(node->child1);  // A
        generateCode(node->child3);  // First B
        generateCode(node->child4);  // Second B
    } else if (node->label == "A") {
        // A -> " t2 | empty
        if (node->token1.instance == "\"") {
            std::string id = processIdentifier(node->token2.instance);
            emit("LOAD 0");
            emit("STORE " + id);
        }
    } else if (node->label == "B") {
        generateCode(node->child1);  // Process S, C, D, E, or G
    } else if (node->label == "C") {
        if (node->token1.instance == "#") {
            // # t2 - Read input and store in identifier
            std::string id = processIdentifier(node->token2.instance);
            emit("READ " + id);
        } else if (node->token1.instance == "!") {
            // ! F - Negate the value
            generateCode(node->child2);
            std::string temp = createTemp();
            emit("MULT -1");
            emit("STORE " + temp);
        }
    } else if (node->label == "D") {
        // $ F - Print value
        if (node->child2->token1.id == t2_tk) {
            // If it's an identifier, print it directly
            std::string id = processIdentifier(node->child2->token1.instance);
            emit("WRITE " + id);
        } else if (node->child2->token1.id == t3_tk) {
            // If it's a number, print it directly
            std::string value = processNumber(node->child2->token1.instance);
            emit("WRITE " + value);
        } else {
            // For expressions, we need to evaluate and store result
            generateCode(node->child2);
            std::string temp = createTemp();
            emit("STORE " + temp);
            emit("WRITE " + temp);
        }
    } else if (node->label == "E") {
        // ' F F F B - Conditional loop
        generateCode(node->child2);  // First F (condition)
        std::string lhs = createTemp();
        emit("STORE " + lhs);

        generateCode(node->child3);  // Second F (comparison)
        std::string rhs = createTemp();
        emit("STORE " + rhs);

        std::string startLabel = newLabel();
        std::string endLabel = newLabel();

        // Check if first > second
        emit("LOAD " + lhs);
        emit("SUB " + rhs);
        emit("BRZ " + endLabel);
        emit("BRNEG " + endLabel);

        emitLabel(startLabel);

        // Get number of iterations
        generateCode(node->child4);
        std::string count = createTemp();
        emit("STORE " + count);

        // Body of loop
        generateCode(node->child5);

        // Decrement counter and check if done
        emit("LOAD " + count);
        emit("SUB 1");
        emit("STORE " + count);
        emit("BRPOS " + startLabel);

        emitLabel(endLabel);
    } else if (node->label == "F") {
        if (node->token1.id == t2_tk) {
            // Identifier
            std::string id = processIdentifier(node->token1.instance);
            emit("LOAD " + id);
        } else if (node->token1.id == t3_tk) {
            // Number
            std::string value = processNumber(node->token1.instance);
            emit("LOAD " + value);
        } else if (node->token1.instance == "&") {
            // Addition
            generateCode(node->child2);
            std::string left = createTemp();
            emit("STORE " + left);

            generateCode(node->child3);
            emit("ADD " + left);
        }
    } else if (node->label == "G") {
        // t2 % F - Assignment
        std::string id = processIdentifier(node->token1.instance);
        generateCode(node->child5);
        emit("STORE " + id);
    }
}

void generateProgram(node_t* root, const std::string& filename) {
    std::ofstream outFile(filename);
    outStream = &outFile;

    generateCode(root);

    emit("STOP");

    // Emit all variable declarations
    for (const auto& var : symbolTable) {
        emit(var.first + " 0");
    }
    for (const auto& temp : tempVars) {
        emit(temp + " 0");
    }

    outFile.close();
}

/* OR1 */