#include "parser.h"
#include "semantics.h"
#include "codegen.h"
#include <iostream>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file> [output_file]\n";
        return 1;
    }

    // Parse the input file
    node_t *root = parser(argc, argv);
    
    // Perform static semantics check
    staticSemantics(root);
    
    // Generate assembly code
    std::string outputFile = (argc > 2) ? argv[2] : "output.asm";
    generateProgram(root, outputFile);
    
    return 0;
}