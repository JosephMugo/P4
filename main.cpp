#include <iostream>

#include "generate.h"
#include "parser.h"
#include "semantics.h"

int main(int argc, char *argv[]) {
    node_t *root = parser(argc, argv);

    staticSemantics(root);

    // Generate assembly code
    std::string outputFile = (argc > 2) ? argv[2] : "output.asm";
    generateProgram(root, outputFile);

    return 0;
}