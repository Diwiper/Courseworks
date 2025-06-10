#ifndef LEXICAL_ANALYSIS_H
#define LEXICAL_ANALYSIS_H

#include <string>
#include <vector>
#include <fstream>
#include <regex>
#include <iostream>

struct Lexeme {
    std::string value;
    std::string type;
    int length;
    int index;
};

bool isWhitespaceOnly(const std::string& str);
std::string toUpperLatin(const std::string& input);
std::vector<Lexeme> analyzeLine(std::string& line);
void writeSentenceStructure(std::ofstream& outFile, const std::vector<Lexeme>& lexemes);
void analyzeFile(const std::string& filename);

#endif 