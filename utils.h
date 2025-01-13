#ifndef UTILS_H
#define UTILS_H

#include "enums.h"
#include <stdexcept>
#include <vector>
#include <string>

// declare keywords as external
extern std::vector<std::string> keywords;

KeywordType stringToKeywordType(const std::string& str);
char stringToChar(const std::string& str);

#endif
