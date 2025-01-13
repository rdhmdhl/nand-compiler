#include "./tokenizer.h"
#include "enums.h"
//#include "utils.h"
#include <iostream> 

int main(){
	Tokenizer tokenizer("ArrayTest.jack");
	 
	while(tokenizer.hasMoreTokens()){
		if(tokenizer.tokenType() == IDENTIFIER){
			std::cout << "Identifier: " << tokenizer.identifier() << std::endl;
		}
		tokenizer.advance();
	}
	std::cout << "no more tokens." << std::endl;
	return -1;
}
