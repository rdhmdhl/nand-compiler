#include "compilationEngine.h"
#include <exception>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include "enums.h"
#include "utils.h"

compilationEngine::compilationEngine(const std::string& inputFilename, const std::string outputFilename){
	std::ifstream tokens;
	tokens.open(inputFilename);

	if(!tokens.is_open()) throw std::runtime_error("Could not open input file: " + inputFilename);
	
	std::string line;
	while(std::getline(tokens, line)){
		tokensBuffer.push_back(line);
	}
	tokens.close();

	OutputFile.open(outputFilename);
	if(!OutputFile.is_open()) throw std::runtime_error("Could not open output file: " + outputFilename);
}

void compilationEngine::compileClass(){
	// wrap xml file with <class> </class>
	OutputFile << "<class>" << std::endl;

	// start with first token
	OutputFile << "<keyword>" << tokensBuffer[currentTokenIndex++] << "</keyword>" << std::endl;
	OutputFile << "<identifier>" << tokensBuffer[currentTokenIndex++] << "</identifier>" << std::endl;
	while(tokensBuffer[currentTokenIndex] != "{"){
		compileClassVarDesc();
	}

	OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;
	compileSubroutine();
}

void compilationEngine::compileClassVarDesc(){
	OutputFile << "<classVarDesc>" << std::endl;

	while(tokensBuffer[currentTokenIndex] != ";"){
		OutputFile << "<keyword>" << tokensBuffer[currentTokenIndex++] << "</keyword>" << std::endl;
		OutputFile << "<keyword>" << tokensBuffer[currentTokenIndex++] << "</keyword>" << std::endl;
		OutputFile << "<identifier>" << tokensBuffer[currentTokenIndex++] << "</identifier>" << std::endl;
		
		if(tokensBuffer[currentTokenIndex] == ","){
			while(tokensBuffer[currentTokenIndex] != ";"){
				OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;
				OutputFile << "<identifier>" << tokensBuffer[currentTokenIndex++] << "</identifier>" << std::endl;
			}
		}
	}

	OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;
	OutputFile << "</classVarDesc>" << std::endl;
}

void compilationEngine::compileSubroutine(){
	OutputFile << "<subroutineDec>" << std::endl;
	// (constructor | method | function), (void | type), (subroutineName), ('('), (parameterList), (')')
	// TODO: UPDATE FOR A CONSTRUCTOR, SECOND ELEMENT COULD BE AN INDENTIFIER
	OutputFile << "<keyword>" << tokensBuffer[currentTokenIndex++] << "</keyword>" << std::endl;
	OutputFile << "<keyword>" << tokensBuffer[currentTokenIndex++] << "</keyword>" << std::endl;
	OutputFile << "<identifier>" << tokensBuffer[currentTokenIndex++] << "</identifier>" << std::endl;
	OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;
	
	compileParameterList();
	
	OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;

	// subroutineBody, '{'
	OutputFile << "<subroutineBody>" << std::endl;
	OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;

	// TODO: KEEP LOOPING THROUGH UNTIL STATEMENTS ARE REACHED
	while(std::find(statements.begin(), statements.end(), tokensBuffer[currentTokenIndex]) == statements.end()){
		compileVarDesc();
	}
	OutputFile << "<statements>" << std::endl;
	// TODO: compile variables here or detect them in main.cpp?
	compileStatements();
	OutputFile << "</statements>" << std::endl;
}

void compilationEngine::compileParameterList(){
	// (type varName), (','), (type varName)*?
	OutputFile << "<parameterList>";

	while(tokensBuffer[currentTokenIndex] != ")"){
		OutputFile << "<keyword>" << tokensBuffer[currentTokenIndex++] << "</keyword>" << std::endl;
		OutputFile << "<identifier>" << tokensBuffer[currentTokenIndex++] << "</identifier>" << std::endl;
		if(tokensBuffer[currentTokenIndex] == ","){
			OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;
		}
	}

	OutputFile << "</parameterList>" << std::endl;
}

void compilationEngine::compileVarDesc(){
	// 'var', type, varName, (',' varName)*, ';'
	//while()
	OutputFile << "<varDec>" << std::endl;
	OutputFile << "<keyword>" << tokensBuffer[currentTokenIndex++] << "</keyword>" << std::endl;

	// TODO: COULD BE ARRAY HERE, WHICH WOULD BE AN IDENTIFIER
	OutputFile << "<keyword>" << tokensBuffer[currentTokenIndex++] << "</keyword>" << std::endl;
	OutputFile << "<identifier>" << tokensBuffer[currentTokenIndex++] << "</identifier>" << std::endl;
	
	while(tokensBuffer[currentTokenIndex] != ";"){
		OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;
		OutputFile << "<keyword>" << tokensBuffer[currentTokenIndex++] << "</keyword>" << std::endl;
	}
	OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;

	OutputFile << "</varDec>" << std::endl;
}

void compilationEngine::compileStatements(){

	while(currentTokenIndex < tokensBuffer.size()){
		if(tokensBuffer[currentTokenIndex] == "if"){
			compileIf();
		} else if (tokensBuffer[currentTokenIndex] == "do"){
			compileDo();
		} else if (tokensBuffer[currentTokenIndex] == "let"){
			compileLet();
		} else if (tokensBuffer[currentTokenIndex] == "return"){
			compileReturn();
		} else {
			compileWhile();
		}
		currentTokenIndex++;
	}

	OutputFile << "end of statements reached..." << std::endl;
}

void compilationEngine::compileIf(){
	// if, '(', expression, ')', '{', statements, '}'
	// (else, '{', statements, '}')?
	OutputFile << "<ifStatement>" << std::endl;
	OutputFile << "<keyword>" << tokensBuffer[currentTokenIndex++] << "</keyword>" << std::endl;
	OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;

	OutputFile << "<expression>" << std::endl;
	OutputFile << "</expression>" << std::endl;
	
	// '{', statement, '}'
	OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;
	compileStatements();	
	OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;

	// move index forward to find the closing ')' for the if statement
	if(tokensBuffer[currentTokenIndex] == "else"){
			OutputFile << "<keyword>" << tokensBuffer[currentTokenIndex++] << "</keyword>" << std::endl;	
			OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;
			OutputFile << "<expression>" << std::endl;
			OutputFile << "</expression>" << std::endl;
			compileStatements();
			OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;
	}
	
	OutputFile << "</ifStatement>" << std::endl;
}

void compilationEngine::compileDo(){
	// 'do', subroutine call, ';'
	OutputFile << "<compileDo>" << std::endl;
	OutputFile << "<keyword>" << tokensBuffer[currentTokenIndex++] << "</keyword>" << std::endl;
	OutputFile << "<identifier>" << tokensBuffer[currentTokenIndex++] << "</identifier>" << std::endl;
	
	// '.', identifier, '(', 
	if(tokensBuffer[currentTokenIndex] == "."){
		OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;
		OutputFile << "<identifier>" << tokensBuffer[currentTokenIndex++] << "</identifier>" << std::endl;
		OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;

		// print string
		if(tokensBuffer[currentTokenIndex++] == "\""){
			OutputFile << "<stringConstant>";
			
			while(tokensBuffer[currentTokenIndex] != "\""){
				OutputFile << tokensBuffer[currentTokenIndex++];
			}

			OutputFile << "</stringConstant>" << std::endl;
			OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;
			OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;
		} else {
			OutputFile << "<expressionList>" << "</expressionList>" << std::endl;
		}
		// closing ')'
		OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;
	}

	// end of do statement';'
	OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;
	OutputFile << "</compileDo>" << std::endl;
}

void compilationEngine::compileLet(){
	// let, varName ( '[' expression ']' )?, '=', expression, ';' 
	OutputFile << "<letStatement>" << std::endl;
	OutputFile << "<keyword>" << tokensBuffer[currentTokenIndex++] << "</keyword>" << std::endl;
	OutputFile << "<identifier>" << tokensBuffer[currentTokenIndex++] << "</identifier>" << std::endl;
	
	if(tokensBuffer[currentTokenIndex] == "("){
		OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;
		compileExpressionList();
		OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;

	}

	// '='
	OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;

	compileExpression();
	
	// expression advances token, finish let with ';'
	OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex] << "</symbol>" << std::endl;
	OutputFile << "</letStatement>" << std::endl;

}

void compilationEngine::compileWhile(){
	// while, '(', expression, ')', ';'
	OutputFile << "<whileStatement>" << std::endl;
	OutputFile << "<keyword>" << tokensBuffer[currentTokenIndex++] << "</keyword>" << std::endl;

	OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;
	OutputFile << "<expression>" << std::endl;
	OutputFile << "</expression>" << std::endl; 
	OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;

	OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;
	compileStatements();
	OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;

	OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;
	OutputFile << "</whileStatement>" << std::endl;
}

void compilationEngine::compileReturn(){
	OutputFile << "<returnStatement>" << std::endl;
	OutputFile << "<keyword>" << tokensBuffer[currentTokenIndex++] << "</keyword>" << std::endl;
	OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;
	OutputFile << "</returnStatement>" << std::endl;

}

void compilationEngine::compileExpression(){
	OutputFile << "<expression>" << std::endl;

	// find the end of the expression, usually notated by ')'
	while(tokensBuffer[currentTokenIndex] != ")"){
		// term advances token
		compileTerm();

		TokenType typeOfToken = tokenType(tokensBuffer[currentTokenIndex]);

		// opporators such as +, -, *, /, &, |, <, >, =
		if(typeOfToken == TokenType::SYMBOL){
			if(tokensBuffer[currentTokenIndex] == ")" || tokensBuffer[currentTokenIndex] == "]" || tokensBuffer[currentTokenIndex] == ";"){
				// do not print anything, do not advance token
				break;
			}
			// output opporator symbol
			OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;
		}
		//currentTokenIndex++;
	}


	OutputFile << "</expression>" << std::endl;
}

void compilationEngine::compileTerm(){
	// integer constant | string constant | keyword constant | var name | var name [
	// expression ] | subroutinecall | '(' expression ')' | unaryOp term
	OutputFile << "<term>" << std::endl; 
	
	TokenType typeOfToken = tokenType(tokensBuffer[currentTokenIndex]);
	
	if(typeOfToken == TokenType::STRING_CONST) OutputFile << "<stringConstant>" << tokensBuffer[currentTokenIndex++] << "</stringConstant>"<< std::endl;

	if(typeOfToken == TokenType::INT_CONST) OutputFile << "<integerConstant>" << tokensBuffer[currentTokenIndex++] << "</integerConstant>" << std::endl;

	if(typeOfToken == TokenType::IDENTIFIER){
		OutputFile << "<identifier>" << tokensBuffer[currentTokenIndex++] << "</identifier>" << std::endl;
		if(tokensBuffer[currentTokenIndex] == "["){
			OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;
			compileExpression();
			OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;
		} else if(tokensBuffer[currentTokenIndex] == "("){
			OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;
			compileExpressionList();
			OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;
	
		} else if(tokensBuffer[currentTokenIndex] == "."){
			// . new ( expressionlist )
			OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;
			OutputFile << "<identifier>" << tokensBuffer[currentTokenIndex++] << "</identifier>" << std::endl;			
			OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;
			compileExpressionList();
			OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;
		}
	}
	OutputFile << "</term>" << std::endl;
}

void compilationEngine::compileExpressionList(){
	// (expression ( ',' expression)* )?	
	OutputFile << "<expressionList>" << std::endl;

	while(tokensBuffer[currentTokenIndex] != ")"){
		// does not move to the next token in compileExpression
		compileExpression();
		if(tokensBuffer[currentTokenIndex] == ","){
			OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex] << "</symbol>" << std::endl;
		}
	}
	OutputFile << "</expressionList>" << std::endl;
	//OutputFile << "<symbol>" << tokensBuffer[currentTokenIndex++] << "</symbol>" << std::endl;
}













