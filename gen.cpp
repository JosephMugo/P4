#include "gen.h"

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

#include "node.h"

int tempCount = 0;
int labelCount = 0;
std::vector<std::string> tempVars;
std::unordered_set<std::string> idSet;
std::ostream *outStream;

std::string createTemp() {
    std::string temp = "T" + std::to_string(tempCount++);
    tempVars.push_back(temp);
    return temp;
}

std::string newLabel() { return "L" + std::to_string(labelCount++); }

void emit(const std::string &code) { (*outStream) << code << "\n"; }

void generateCode(node_t *node) {
    if (!node) return;

    if (node->label == "S") {
        generateCode(node->child1);
        generateCode(node->child3);
        generateCode(node->child4);
    } else if (node->label == "A") {
        if (node->token1.instance == "\"") {
            std::string id = node->token2.instance;
            if (idSet.insert(id).second) {
                emit("LOAD 0");
                emit("STORE " + id);
            }
        }
    } else if (node->label == "B") {
        generateCode(node->child1);
    } else if (node->label == "C") {
        if (node->token1.instance == "#") {
            std::string id = node->token2.instance;
            idSet.insert(id);
            emit("READ " + id);
        } else if (node->token1.instance == "!") {
            generateCode(node->child2);
            std::string temp = createTemp();
            emit("MULT -1");
            emit("STORE " + temp);
        }
    } else if (node->label == "D") {
        generateCode(node->child2);
        std::string temp = createTemp();
        emit("LOAD " + temp);
        emit("WRITE");
    } else if (node->label == "E") {
        generateCode(node->child2);
        std::string lhs = createTemp();
        emit("STORE " + lhs);

        generateCode(node->child3);
        std::string rhs = createTemp();
        emit("STORE " + rhs);

        std::string startLabel = newLabel();
        std::string endLabel = newLabel();

        emit("LOAD " + lhs);
        emit("SUB " + rhs);
        emit("BRZ " + endLabel);
        emit("BRNEG " + endLabel);

        emit(startLabel + ":");

        generateCode(node->child4);
        std::string count = createTemp();
        emit("STORE " + count);

        generateCode(node->child5);

        emit("LOAD " + count);
        emit("SUB 1");
        emit("BRPOS " + startLabel);

        emit(endLabel + ":");
    } else if (node->label == "F") {
        if (node->token1.id == t2_tk || node->token1.id == t3_tk) {
            std::string val = node->token1.instance;
            std::string temp = createTemp();
            emit("LOAD " + val);
            emit("STORE " + temp);
        } else if (node->token1.instance == "&") {
            generateCode(node->child2);
            std::string left = createTemp();
            emit("STORE " + left);

            generateCode(node->child3);
            std::string right = createTemp();
            emit("STORE " + right);

            std::string result = createTemp();
            emit("LOAD " + left);
            emit("ADD " + right);
            emit("STORE " + result);
        }
    } else if (node->label == "G") {
        std::string id = node->token1.instance;
        generateCode(node->child5);
        std::string temp = createTemp();
        emit("LOAD " + temp);
        emit("STORE " + id);
    }
}

void generateProgram(node_t *root, const std::string &filename) {
    std::ofstream outFile(filename);
    outStream = &outFile;

    generateCode(root);

    emit("STOP");

    for (const auto &id : idSet) {
        emit(id + " 0");
    }
    for (const auto &temp : tempVars) {
        emit(temp + " 0");
    }

    outFile.close();
}
