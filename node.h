#ifndef NODE_H
#define NODE_H
#include <string>

#include "token.h"

typedef struct node_t {
    std::string label;
    Token token1;
    Token token2;
    node_t *child1;
    node_t *child2;
    node_t *child3;
    node_t *child4;
    node_t *child5;
} node_t;

node_t *getNode(std::string label);

#endif