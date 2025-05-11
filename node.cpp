#include "node.h"

#include <string>

node_t *getNode(std::string label) {
    node_t *holder = new node_t;
    holder->label = label;
    holder->token1.id = NULL_TK;
    holder->token2.id = NULL_TK;
    holder->child1 = nullptr;
    holder->child2 = nullptr;
    holder->child3 = nullptr;
    holder->child4 = nullptr;
    holder->child5 = nullptr;
    return holder;
}
