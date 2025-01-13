#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <vector>
#include <fstream>
#include "enums.h"
#include "utils.h"

class Tokenizer {
private:
    std::ifstream JackFile;
    std::string currentToken;
    std::vector<std::string> tokens;
    size_t currentTokenIndex = 0;
    bool hasMore = true;

    bool isSymbol(char c);

public:
    // Constructor
    Tokenizer(const std::string& inputFile);

    // Destructor
    ~Tokenizer();

    // Public methods
    void printAll();
    bool hasMoreTokens();
    void advance();
    TokenType tokenType();
    KeywordType keyWord();
    char symbol(const std::string& token);
    bool isValidIdenitier(const std::string& token);
    std::string identifier();
    int intVal(const std::string& token);
    std::string stringVal(const std::string& token);
};

#endif
