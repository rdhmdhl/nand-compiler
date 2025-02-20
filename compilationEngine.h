#ifndef COMPILATIONENGINE_H
#define COMPILATIONENGINE_H
#include <iostream>
#include <fstream>
#include "utils.h"
#include "Tokenizer.h"

class compilationEngine{
private:
	std::vector<std::string> tokensBuffer;
	std::ofstream OutputFile;
	size_t currentTokenIndex = 0;
	std::string line;
public:
	compilationEngine(const std::string& inputFilename, const std::string outputFilename);
	void compileClass();
	void compileClassVarDesc();
	void compileSubroutine();
	void compileParameterList();
	void compileVarDesc();
	void compileStatements();
	void compileDo();
	void compileLet();
	void compileWhile();
	void compileReturn();
	void compileIf();
	void compileExpression();
	void compileTerm();
	void compileExpressionList();
};

#endif
