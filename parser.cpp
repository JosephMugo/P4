#include "parser.h"

#include <fstream>
#include <iomanip>
#include <iostream>

#include "node.h"
#include "scanner.h"
#include "token.h"

int lineNumber = 1;
char nextChar = ' ';
std::istream *input_stream;
Token currentToken;
int outputWidth = 10;

/**
 * @brief Provides token string based on token id
 * @param id token id
 * @return string associated to token id
 */
std::string tokenToString(tokenID id) {
    switch (id) {
        case t1_tk:
            return "t1";
        case t2_tk:
            return "t2";
        case t3_tk:
            return "t3";
        case EOFTk:
            return "EOFTk";
        case ERROR_TK:
            return "ERRORTK";
        case EMTPY_TK:
            return "empty";
        default:
            return "UNKNOWN";
    }
}

/**
 * @brief Print token error
 * @param tk Token
 */
void printError(Token tk) {
    std::cerr << tk.instance << std::setw(outputWidth);
    std::cerr << tk.lineNumber << "\n";
}

/**
 * @brief Iteratively calls scanner and parse tokens
 * @param argc command line arguments count
 * @param argv command line arguments
 */
node_t *parser(int argc, char *argv[]) {
    std::ifstream file_input;

    if (argc > 1) {
        // user provides file
        file_input.open(argv[1]);
        if (!file_input.is_open()) {
            std::cerr << "Program error: Could not open file " << argv[1]
                      << "\n";
            exit(1);
        }
        input_stream = &file_input;
    } else {
        // No file provided, use the standard input (cin)
        input_stream = &std::cin;
    }

    // root node
    node_t *root;

    // Get first token
    currentToken = scanner();
    // call S function to start parsing
    root = S();

    if (currentToken.id == EOFTk) {
        if (file_input.is_open()) {
            file_input.close();
        }
        return root;
    }

    std::cerr << "[Error processing <S>, ";
    std::cerr << "Expected [\"] or [(] from source, ln: ";
    std::cerr << currentToken.lineNumber << "]\n";
    exit(1);
}

/**
 * @brief Function that represents production rule: S -> A ( B B )
 * opening and closing squa+re brackets represent what is being processed
 */
node_t *S() {
    node_t *sTreeRoot = getNode("S");
    // S -> [A] ( B B )
    // Call A production rule: A -> " t2 | empty
    sTreeRoot->child1 = A();
    // Validate token '(': S -> A [(] B B )
    if (!currentToken.instance.empty() && currentToken.instance.at(0) == '(') {
        sTreeRoot->token1 = currentToken;
        // Get next token
        currentToken = scanner();
        // Call B production rule: S -> A ( [B] B )
        sTreeRoot->child3 = B();
        // Call B production rule: S -> A ( B [B] )
        sTreeRoot->child4 = B();
        // Validate token ')': S -> A ( B B [)]
        if (!currentToken.instance.empty() &&
            currentToken.instance.at(0) == ')') {
            sTreeRoot->token2 = currentToken;
            currentToken = scanner();
            return sTreeRoot;
        }
        // Language expecting ')'
        // S -> A ( B B [)]
        std::cerr << "[Error processing <S>, ";
        std::cerr << "Expected [)] from source, ln: ";
        std::cerr << currentToken.lineNumber << "]\n";
        exit(1);
    }
    // Language expecting '('
    // S -> A [(] B B )
    std::cerr << "[Error processing <S>, ";
    std::cerr << "Expected [(] from source, ln: ";
    std::cerr << currentToken.lineNumber << "]\n";
    exit(1);
}

/**
 * @brief Function that represents production rule: A -> " t2 | empty
 * opening and closing square brackets represent what is being processed
 */
node_t *A() {
    node_t *aTreeRoot = getNode("A");
    // A -> ["] t2
    if (!currentToken.instance.empty() && currentToken.instance.at(0) == '"') {
        aTreeRoot->token1 = currentToken;
        currentToken = scanner();
        // A -> " [t2]
        if (currentToken.id == t2_tk) {
            aTreeRoot->token2 = currentToken;
            currentToken = scanner();
            return aTreeRoot;
        }
        // Language expecting t2 token
        // A -> " [t2] | empty
        std::cerr << "[Error processing <A>, ";
        std::cerr << "Expected [t2] token from source, ln: ";
        std::cerr << currentToken.lineNumber << "]\n";
        exit(1);
    }
    // A -> empty
    aTreeRoot->token1.id = EMTPY_TK;
    return aTreeRoot;
}

/**
 * @brief Function that represents production rule: B -> S | C | D | E | G
 * opening and closing square brackets represent what is being processed
 */
node_t *B() {
    node_t *bTreeRoot = getNode("B");
    // B -> S
    // Uses S First Set to identify if S is being evaluated
    if (!currentToken.instance.empty() && currentToken.instance.at(0) == '"' ||
        !currentToken.instance.empty() && currentToken.instance.at(0) == '(') {
        bTreeRoot->child1 = S();
        return bTreeRoot;
    }
    // B -> C
    // Uses C First Set to identify if C is being evaluated
    if (!currentToken.instance.empty() && currentToken.instance.at(0) == '#' ||
        !currentToken.instance.empty() && currentToken.instance.at(0) == '!') {
        bTreeRoot->child1 = C();
        return bTreeRoot;
    }
    // B -> D
    // Uses D First Set to identify if D is being evaluated
    if (!currentToken.instance.empty() && currentToken.instance.at(0) == '$') {
        bTreeRoot->child1 = D();
        return bTreeRoot;
    }
    // B -> E
    // Uses E First Set to identify if E is being evaluated
    if (!currentToken.instance.empty() && currentToken.instance.at(0) == '\'') {
        bTreeRoot->child1 = E();
        return bTreeRoot;
    }
    // B -> G
    // Uses G First Set to identify if G is being evaluated
    if (currentToken.id == t2_tk) {
        bTreeRoot->child1 = G();
        return bTreeRoot;
    }
    // Language expecting: B -> S | C | D | E | G
    // B -> [S] | [C] | [D] | [E] | [G]
    std::cerr << "[Error processing <B>, ";
    std::cerr << "Expected [S] | [C] | [D] | [E] | [G] non-terminal from "
                 "source, ln: ";
    std::cerr << currentToken.lineNumber << "]\n";
    exit(1);
}

/**
 * @brief Function that represents production rule: C -> # t2 | ! F
 * opening and closing square brackets represent what is being processed
 */
node_t *C() {
    node_t *cTreeRoot = getNode("C");
    // C -> [#] t2
    if (!currentToken.instance.empty() && currentToken.instance.at(0) == '#') {
        cTreeRoot->token1 = currentToken;
        currentToken = scanner();
        // C -> # [t2]
        if (currentToken.id == t2_tk) {
            cTreeRoot->token2 = currentToken;
            currentToken = scanner();
            return cTreeRoot;
        }
        // Language expecting t2 token
        // C -> # [t2]
        std::cerr << "[Error processing <C>, ";
        std::cerr << "Expected [t2] token from source, ln: ";
        std::cerr << currentToken.lineNumber << "]\n";
        exit(1);
    }
    // C -> [!] F
    if (!currentToken.instance.empty() && currentToken.instance.at(0) == '!') {
        cTreeRoot->token1 = currentToken;
        currentToken = scanner();
        // C -> # t2 | ! [F]
        cTreeRoot->child2 = F();
        return cTreeRoot;
    }
    // Language expecting '#' or '!'
    // C -> [#] t2 | [!] F
    std::cerr << "[Error processing <C>, ";
    std::cerr << "Expected [#] or [!] from source, ln: ";
    std::cerr << currentToken.lineNumber << "]\n";
    exit(1);
}

/**
 * @brief Function that represents production rule: D -> $ F
 * opening and closing square brackets represent what is being processed
 */
node_t *D() {
    node_t *dTreeRoot = getNode("D");
    // D -> [$] F
    if (!currentToken.instance.empty() && currentToken.instance.at(0) == '$') {
        dTreeRoot->token1 = currentToken;
        currentToken = scanner();
        // D -> $ [F]
        dTreeRoot->child2 = F();
        return dTreeRoot;
    }
    // Language expecting '$'
    // D -> [$] F
    std::cerr << "[Error processing <D>, ";
    std::cerr << "Expected [$] from source, ln: ";
    std::cerr << currentToken.lineNumber << "]\n";
    exit(1);
}

/**
 * @brief Function that represents production rule: E -> ' F F F B
 * opening and closing square brackets represent what is being processed
 */
node_t *E() {
    node_t *eTreeRoot = getNode("E");
    // E -> ['] F F F B
    if (!currentToken.instance.empty() && currentToken.instance.at(0) == '\'') {
        eTreeRoot->token1 = currentToken;
        currentToken = scanner();
        // E -> ' [F] F F B
        eTreeRoot->child2 = F();
        // E -> ' F [F] F B
        eTreeRoot->child3 = F();
        // E -> ' F F [F] B
        eTreeRoot->child4 = F();
        // E -> ' F F F [B]
        eTreeRoot->child5 = B();
        return eTreeRoot;
    }
    // Language expecting '
    // E -> ['] F F F B
    std::cerr << "[Error processing <E>, ";
    std::cerr << "Expected ['] from source, ln: ";
    std::cerr << currentToken.lineNumber << "]\n";
    exit(1);
}

/**
 * @brief Function that represents production rule: F -> t2 | t3 | & F F
 * opening and closing square brackets represent what is being processed
 */
node_t *F() {
    node_t *fTreeRoot = getNode("F");
    // F -> [t2] | [t3]
    if (currentToken.id == t2_tk || currentToken.id == t3_tk) {
        fTreeRoot->token1 = currentToken;
        currentToken = scanner();
        return fTreeRoot;
    }
    // F -> [&] F F
    if (!currentToken.instance.empty() && currentToken.instance.at(0) == '&') {
        fTreeRoot->token1 = currentToken;
        currentToken = scanner();
        // F -> & [F] F
        fTreeRoot->child2 = F();
        // F -> & F [F]
        fTreeRoot->child3 = F();
        return fTreeRoot;
    }
    // Language expecting t2, t3 token or '&'
    // F -> [t2] | [t3] | [&] F F
    std::cerr << "[Error processing <F>, ";
    std::cerr << "Expected [t2], [t3] token or [&] from source, ln: ";
    std::cerr << currentToken.lineNumber << "]\n";
    exit(1);
}

/**
 * @brief Function that represents production rule: G -> t2 % F
 * opening and closing square brackets represent what is being processed
 */
node_t *G() {
    node_t *gTreeRoot = getNode("G");
    // G -> [t2] % F
    if (currentToken.id == t2_tk) {
        gTreeRoot->token1 = currentToken;
        currentToken = scanner();
        // G -> t2 [%] F
        if (!currentToken.instance.empty() &&
            currentToken.instance.at(0) == '%') {
            gTreeRoot->token2 = currentToken;
            currentToken = scanner();
            // G -> t2 % [F]
            gTreeRoot->child5 = F();
            return gTreeRoot;
        }
        // Language expecting '%'
        // G -> t2 [%] F
        std::cerr << "[Error processing <G>, ";
        std::cerr << "Expected [%] token from source, ln: ";
        std::cerr << currentToken.lineNumber << "]\n";
        exit(1);
    }
    // Language expecting t2 token
    // G -> [t2] % F
    std::cerr << "[Error processing <G>, ";
    std::cerr << "Expected [t2] token from source, ln: ";
    std::cerr << currentToken.lineNumber << "]\n";
    exit(1);
}
