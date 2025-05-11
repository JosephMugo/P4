#include "generate.h"

#include <cstdio>
#include <iomanip>
#include <iostream>
#include <string>

void traverseTree(node_t *root, int level);

void traverseTree(node_t *root, int level) {
    if (root == nullptr) return;
    std::cout << std::setw(level * 4) << "" << root->label << "\n";
    traverseTree(root->child1, level + 1);
    if (root->token1.id != NULL_TK) {
        std::cout << std::setw((level + 1) * 4) << ""
                  << tokenToString(root->token1.id) << " "
                  << root->token1.instance << "\n";
        if (root->token1.id == t2_tk) {
            std::cout << "t2 token" << "\n";
        }
        if (root->token1.id == t1_tk) {
            std::cout << "t1 token" << "\n";
        }
    }
    traverseTree(root->child2, level + 1);
    traverseTree(root->child3, level + 1);
    traverseTree(root->child4, level + 1);
    if (root->token2.id != NULL_TK) {
        std::cout << std::setw((level + 1) * 4) << ""
                  << tokenToString(root->token2.id) << " "
                  << root->token2.instance << "\n";
        if (root->token1.id == t2_tk) {
            std::cout << "t2 token" << "\n";
        }
        if (root->token1.id == t1_tk) {
            std::cout << "t1 token" << "\n";
        }
    }
    traverseTree(root->child5, level + 1);
}

void generateAssembly(node_t *root) { traverseTree(root, 0); }