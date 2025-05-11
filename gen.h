#ifndef GEN_H
#define GEN_H

#include <fstream>
#include <string>

#include "node.h"  // for node_t

/**
 * @brief Generates code from the parse tree starting at root node
 * @param root The root node of the parse tree
 * @param filename The name of the output file where code will be written
 */
void generateProgram(node_t *root, const std::string &filename);

/**
 * @brief Recursively generate code from the given node
 * @param node The current node being processed
 * @param out The output stream to write generated code
 */
void generateCode(node_t *node, std::ofstream &out);

/**
 * @brief Emits a single line of code to the output stream
 * @param line The line to emit
 * @param out The output stream to write to
 */
void emit(const std::string &line, std::ofstream &out);

/**
 * @brief Emits label to output stream
 * @param label Label string (e.g., L1:)
 * @param out Output stream to write to
 */
void emitLabel(const std::string &label, std::ofstream &out);

/**
 * @brief Generates a new temporary variable name
 * @return The new temporary variable name
 */
std::string newTemp();

/**
 * @brief Generates a new label name for control flow
 * @return The new label name
 */
std::string newLabel();

/**
 * @brief Collects and emits all declared identifiers and temporaries
 * @param out Output stream to write declarations
 */
void emitVariableDeclarations(std::ofstream &out);

#endif  // CODE_GENERATOR_H
