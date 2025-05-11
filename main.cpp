#include "parser.h"
#include "semantics.h"

int main(int argc, char *argv[]) {
    node_t *root = parser(argc, argv);
    staticSemantics(root);
    return 0;
}