#ifndef GENERATE_H
#define GENERATE_H

#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "node.h"

// Global variables for code generation
extern int tempCount;
extern int labelCount;
extern std::vector<std::string> tempVars;
extern std::unordered_map<std::string, std::string> symbolTable;
extern std::ostream* outStream;

// Helper functions
std::string createTemp();
std::string newLabel();
void emit(const std::string& code);
void emitLabel(const std::string& label);

// Main code generation function
void generateCode(node_t* node);

// Process number tokens (t3)
std::string processNumber(const std::string& num);

// Process identifier tokens (t2)
std::string processIdentifier(const std::string& id);

// Generate complete program
void generateProgram(node_t* root, const std::string& filename);

#endif