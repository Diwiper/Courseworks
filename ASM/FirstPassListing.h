#ifndef FIRST_PASS_LISTING_H
#define FIRST_PASS_LISTING_H

#include "Lexical_analysis.h"
#include <map>
#include <fstream>

struct Label {
    std::string name;
    std::string segment;
    int offset;
};

struct Variable {
    std::string name;
    std::string segment;
    int offset;
    std::string type;
    int size; 
};

struct Segment {
    std::string name;
    int currentOffset;
    std::vector<Variable> variables;
    std::vector<Label> labels;
};

struct AssemblyError {
    int lineNumber;
    std::string message;
};

class FirstPassListing {
private:
    std::map<std::string, Segment> segments;
    std::string currentSegment;
    int address;
    std::map<std::string, std::string> variableTypes; 
    std::map<std::string, int> variableSizes;
    std::vector<AssemblyError> errors;

    int calculateSize(const std::vector<Lexeme>& lexemes);
    std::string formatAddress(int address);
    void processLine(const std::string& line, int lineNumber, std::ofstream& outputFile);
    void addError(int lineNumber, const std::string& message);
    bool validateInstruction(const std::vector<Lexeme>& lexemes, int lineNumber);
    bool validateVariable(const std::vector<Lexeme>& lexemes, int lineNumber);
    bool validateSegment(const std::vector<Lexeme>& lexemes, int lineNumber);
    bool validateLabel(const std::vector<Lexeme>& lexemes, int lineNumber);

public:
    FirstPassListing();
    void generateListing(const std::string& inputFile, const std::string& outputFile);
    void printVariablesList(std::ofstream& outputFile);
    void printLabelsList(std::ofstream& outputFile);
    void printErrorsList(std::ofstream& outputFile);
    std::string extractVariableName(const std::vector<Lexeme>& lexemes, size_t startIndex);
    std::string analyzeOperandType(const std::vector<Lexeme>& lexemes, size_t startIndex);
    bool is32BitVariable(const std::string& varName);
    std::string getVariableSegment(const std::string& varName);
    const std::map<std::string, Segment>& getSegments() const {return segments;}
};

#endif 