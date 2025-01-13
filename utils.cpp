#include "utils.h"

KeywordType stringToKeywordType(const std::string& str) {
    auto it = std::find(keywords.begin(), keywords.end(), str);
    if (it != keywords.end()) {
	size_t index = std::distance(keywords.begin(), it);
	return static_cast<KeywordType>(index);
    }
    throw std::runtime_error("Keyword '" + str + "' not found in list of known keywords.");
}

// Vector to hold all keywords
std::vector<std::string> keywords = {
    "class", "method", "function",
    "constructor", "int", "boolean",
    "char", "void", "var", "static",
    "field", "let", "do", "if", "else",
    "while", "return", "true", "false", "this", 
    "nil" // Corresponds to NIL in the enum
};

char stringToChar(const std::string& str) {
    // define the set of symbols we're interested in
    const std::string symbols = "(){}[].,;+-X/&|<>=~?!";
    
    if (str.length() == 1) {
	// if the input is already a single symbol, return it as char
	size_t found = symbols.find(str[0]);
	if (found != std::string::npos) {
	    return str[0];
	}
    } else if (str.length() == 0) {
	throw std::invalid_argument("Empty string cannot be converted to symbol.");
    } else {
	// if the string length is more than 1, check if it's one of our symbols
	if (symbols.find(str) != std::string::npos) {
	    // we only expect single-character symbols in our set, so this is an error
	    throw std::invalid_argument("String '" + str + "' is not a single character symbol.");
	}
    }
    // if we've reached here, the string doesn't match any of our symbols
    throw std::invalid_argument("String '" + str + "' is not a recognized symbol.");
}
