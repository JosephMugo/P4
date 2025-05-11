#include "semantics.h"

#include <iostream>
#include <string>
#include <unordered_map>

#include "token.h"

void traverseTree(node_t *root);

// Map data structure to hold table
// start OR1
std::unordered_map<std::string, Token> sTable;
// end OR1
// Variable to let program know next token is variable after # or "
bool isNextVariable = false;

/**
 * @brief Insert instance into map data structure
 * @param s string representing t2 instance
 * @param t token being inserted
 */
void insert(std::string s, Token t) {
    auto result = sTable.insert({s, t});
    // Validate insert
    if (!result.second) {
        std::cerr << "Error";
        exit(1);
    }
}

/**
 * @brief Validate variable not in table
 * @param s string representing t2 instance
 */
bool verify(std::string s) {
    if (sTable.find(s) != sTable.end()) {
        return true;
    }
    return false;
}

/**
 * @brief Preorder traverse and process variables
 * @param root pointer to top root node
 */
void traverseTree(node_t *root) {
    if (root == nullptr) return;
    traverseTree(root->child1);
    if (root->token1.id != NULL_TK) {
        // Check if previous state was # or "
        if (isNextVariable) {
            // Check if variable already exist
            if (verify(root->token1.instance)) {
                std::cerr << "Error: Redefining a variable";
                std::cerr << ": " << tokenToString(root->token1.id)
                          << " instance " << root->token1.instance << " ln#->"
                          << root->token1.lineNumber;
                exit(1);
            }
            // Insert variable
            insert(root->token1.instance, root->token1);
        }
        // Check if token is # or " for variable declaration
        if (root->token1.id != EMTPY_TK) {
            if (root->token1.instance.at(0) == '#' ||
                root->token1.instance.at(0) == '"') {
                isNextVariable = true;
            } else {
                isNextVariable = false;
            }
        }
        // Check if token exist, when referenced
        if (root->token1.id == t2_tk) {
            if (!verify(root->token1.instance)) {
                std::cerr << "Error: Undefined Variable";
                std::cerr << ": " << tokenToString(root->token1.id)
                          << " instance " << root->token1.instance << " ln#->"
                          << root->token1.lineNumber;
                exit(1);
            }
        }
    }
    traverseTree(root->child2);
    traverseTree(root->child3);
    traverseTree(root->child4);
    if (root->token2.id != NULL_TK) {
        // Check if previous state was # or "
        if (isNextVariable) {
            // Check if variable already exist
            if (verify(root->token2.instance)) {
                std::cerr << "Error: Redefining a variable";
                std::cerr << ": " << tokenToString(root->token2.id)
                          << " instance " << root->token2.instance << " ln#->"
                          << root->token2.lineNumber;
                exit(1);
            }
            // Insert variable
            insert(root->token2.instance, root->token2);
        }
        // Check if token is # or " for variable declaration
        if (root->token2.id != EMTPY_TK) {
            if (root->token2.instance.at(0) == '#' ||
                root->token2.instance.at(0) == '"') {
                isNextVariable = true;
            } else {
                isNextVariable = false;
            }
        }
        // Check if token exist, when referenced
        if (root->token2.id == t2_tk) {
            if (!verify(root->token2.instance)) {
                std::cerr << "Error: Undefined Variable";
                std::cerr << ": " << tokenToString(root->token2.id)
                          << " instance " << root->token2.instance << " ln#->"
                          << root->token2.lineNumber;
                exit(1);
            }
        }
    }
    traverseTree(root->child5);
}

void staticSemantics(node_t *root) { traverseTree(root); }