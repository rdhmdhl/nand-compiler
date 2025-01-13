#include "Tokenizer.h"
#include <cctype>
#include <iostream>
#include <sstream>

// constructor, opens the input file
Tokenizer::Tokenizer(const std::string& inputFile){
	JackFile.open(inputFile);
	std::string line, token;
	bool inMultiLineComments = false;
	bool inString = false;

	if(!JackFile.is_open()){
		throw std::runtime_error("Could not open the input file: " + inputFile);
	}

	
	while(std::getline(JackFile, line)){
		// remove leading and trailing whitespace
		line.erase(0, line.find_first_not_of(" \t"));
		line.erase(line.find_last_not_of(" \t") + 1);

		// remove carriage return for windows style line endings
		line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

		size_t commentStart = line.find("//");
		size_t multiLineStart = line.find("/**");
		size_t multiLineEnd = line.find("*/");

		// handle inline comments
		if(commentStart != std::string::npos){
			line = line.substr(0, commentStart);
		}

		// handle multi-line comments
		if(inMultiLineComments){
			if(multiLineEnd != std::string::npos){
				// end of multi-line comment found, skip this line
				line = line.substr(multiLineEnd + 2);
				inMultiLineComments = false;
			} else {
				continue;
			}
		}
		
		if(multiLineStart != std::string::npos){
			// start of multi line comment found
			if(multiLineEnd == std::string::npos){
				inMultiLineComments = true;
				continue;
			} else {
				// comment starts and ends on a single line
				line = line.substr(0, multiLineStart) + line.substr(multiLineEnd + 2);
			}
		}

		// skip processing if line is empty or contains whitespace
		if(line.empty() || line.find_first_not_of(" /t") == std::string::npos){
			continue;
		}

		// process the line for tokens, add to tokens vector
		std::istringstream iss(line);
		for(size_t i = 0; i < line.length(); ++i){
			if(inString){
				// inside a string, keep adding characters until we find closing quote
				if(line[i] == '"' && (i == 0 || line[i-1] != '\\')){
					inString = false;
					token += line[i];
					tokens.push_back(token);
					token.clear();
				} else {
					token += line[i];
				}	
			} else if (line[i] == '"'){
				// start of string
				if(!token.empty()){
					tokens.push_back(token);
				}
				inString = true;
				token += line[i];

			} else if(isSymbol(line[i])){
				// if we have a token built up, add it before the symbol
				if(!token.empty()){
					tokens.push_back(token);
					token.clear();
				}
				// the symbol itself is a token
				tokens.push_back(std::string(1, line[i]));

			} else if (line[i] == ' ' || line[i] == '\t'){
				// space or tab, end the current token if one exists
				if(!token.empty()){
					tokens.push_back(token);
					token.clear();
				}
			} else {
				// build up the token
				token += line[i];
			}
		}
		// add any remaining token after processing the line
		if(!token.empty()){
			tokens.push_back(token);
		}
	}

	return;
}

void Tokenizer::printAll(){
	for(size_t i = 0; i < tokens.size(); ++i){
		std::cout << tokens[i] << std::endl;
	}
}

// deconstructor
Tokenizer::~Tokenizer(){
	if(JackFile.is_open()) JackFile.close();
}

bool Tokenizer::isSymbol(char c){
	const std::string symbols = "(){}[].,;+-X/&|<>=~?!";
	return symbols.find(c) != std::string::npos;
}

bool Tokenizer::hasMoreTokens(){
	return currentTokenIndex < tokens.size();
}

void Tokenizer::advance(){
	if(hasMore){
		currentTokenIndex++;
		hasMore = currentTokenIndex < tokens.size();
	}
}

TokenType Tokenizer::tokenType(){
	std::string token = tokens[currentTokenIndex];

	if(token.empty()){
		throw std::runtime_error("Empty token encountered. Cannot identify token type: " + token);
	}

	// check if token if a keyword
	if(std::find(keywords.begin(), keywords.end(), token) != keywords.end()){
		return KEYWORD; 
	}

	// check if token is a symbol
	if(token.length() == 1 && isSymbol(token[0])){
		return SYMBOL;
	}

	// check if token is an integer
	bool is_integer = true;
	for(char c : token){
		if(!std::isdigit(c)){
			is_integer = false;
			break;
		}
	}
	if(is_integer){
		return INT_CONST;
	}

	// check if token is a string
	if(token.length() >= 2 && token.front() == '"' && token.back() == '"'){
		return STRING_CONST;
	}

	return IDENTIFIER;
}

KeywordType Tokenizer::keyWord(){
	// return an iterator pointing to the first occurence of the specified value in the range [keywords.begin(), keywords.end()]
	auto it = std::find(keywords.begin(), keywords.end(), tokens[currentTokenIndex]);
	if(it != keywords.end()){
		// dereference the iterator to get string
		return stringToKeywordType(*it);
	}
	throw std::runtime_error("Token: " + tokens[currentTokenIndex] + " is not a recognized keyword.");
}

char Tokenizer::symbol(const std::string& token){
	return stringToChar(token);
}

bool Tokenizer::isValidIdenitier(const std::string& token){
	if(token.empty()) return false;

	if(std::isdigit(token[0])) return false;

	// check all characters are letters, digits or underscore
	for(char c : token){
		if(!std::isalnum(c) && c != '_') return false;
	}

	return true;
}

std::string Tokenizer::identifier(){
	std::string token = tokens[currentTokenIndex];
	if(!isValidIdenitier(token)){
		throw std::runtime_error("Token: " + token + " is not a valid identifier.");
	}
	return token;
}

int Tokenizer::intVal(const std::string& token) {
    try {
	return std::stoi(token);
    } catch (const std::invalid_argument& e) {
	throw std::runtime_error("Token: " + token + " is not a valid integer.");
    } catch (const std::out_of_range& e) {
	throw std::runtime_error("Token: " + token + " represents an integer out of range.");
    }
}

std::string Tokenizer::stringVal(const std::string& token) {
    // if you need to check for specific string formats, like ensuring it's quoted:
    if (token.length() < 2 || token.front() != '"' || token.back() != '"') {
	throw std::runtime_error("Token: " + token + " is not a valid string literal.");
    }
	// or return token.substr(1, token.length() - 2) if you want to remove quotes
	return token;
}
