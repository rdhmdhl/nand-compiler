#include "./Tokenizer.h"
#include "./compilationEngine.h"
//#include "enums.h"
//#include "utils.h"
#include <iostream> 

// TODO: collect tokens inside a vector, use vector within the tokenizer and compilation engine

int main(){
	Tokenizer tokenizer("ArrayTest.jack");
	compilationEngine compilationEngine("TokenFile.txt", "Tokens.xml");
	compilationEngine.compileClass();
	std::cout << "no more tokens." << std::endl;
	return -1;
}
