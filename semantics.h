#ifndef SEMANTICS_H
#define SEMNATICS_H
#include <string>

#include "node.h"

void insert(std::string s);
bool verify(std::string s);
void staticSemantics(node_t *root);

#endif