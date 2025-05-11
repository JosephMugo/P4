#ifndef PARSER_H
#define PARSER_H
#include <node.h>
#include <token.h>

node_t *parser(int argc, char *argv[]);
void printError(Token tk);
node_t *S();
node_t *A();
node_t *B();
node_t *C();
node_t *D();
node_t *E();
node_t *F();
node_t *G();

#endif