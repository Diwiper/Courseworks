#ifndef SECOND_PASS_LISTING_H
#define SECOND_PASS_LISTING_H
#include "FirstPassListing.h"
#include "Lexical_analysis.h"
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iomanip>
#include <sstream>

struct MachineCode {
    std::vector<unsigned char> bytes;
    std::string opcode;
    std::string operands;
    std::string machineCodePrefix;
};

struct Error {
    int lineNumber;
    std::string errorMessage;
    std::string context;
};

class SecondPassListing {
private:
    const FirstPassListing& firstPass;
    std::map<std::string, int> labelAddresses;
    std::string currentSegment;
    int currentAddress;
    std::ofstream outputFile;
    std::map<std::string, int> registers;
    std::vector<Error> errors;

    void initializeRegisters();
    std::string formatAddress(int address);
    std::string formatHexByte(unsigned char byte);
    std::string formatMachineCode(const std::vector<unsigned char>& bytes, bool need_ref, int maxWidth);
    MachineCode generateMachineCode(const std::vector<Lexeme>& lexemes);
    MachineCode generatePushInstruction(const std::vector<Lexeme>& lexemes);
    MachineCode generateBtInstruction(const std::vector<Lexeme>& lexemes);
    MachineCode generateLeaInstruction(const std::vector<Lexeme>& lexemes);
    MachineCode generateOutInstruction(const std::vector<Lexeme>& lexemes);
    MachineCode generateCmpInstruction(const std::vector<Lexeme>& lexemes);
    MachineCode generateJmpInstruction(const std::vector<Lexeme>& lexemes);
    MachineCode generateCliInstruction(const std::vector<Lexeme>& lexemes);
    int getRegisterCode(const std::string& regName);
    int getSegmentRegisterPrefix(const std::string& segRegName);
    long getImmediateValue(const std::string& immValue);
    void processLine(const std::string& line, int lineNumber);
    void processSegmentStart(const std::vector<Lexeme>& lexemes);
    void processSegmentEnd(const std::vector<Lexeme>& lexemes);
    void processInstruction(const std::vector<Lexeme>& lexemes, int lineNumber);
    void processVariable(const std::vector<Lexeme>& lexemes, int lineNumber);
    void processLabel(const std::vector<Lexeme>& lexemes, int lineNumber);
    std::vector<unsigned char> calculateVariableData(const std::string& type, const std::vector<Lexeme>& lexemes, int lineNumber);
    long evaluateExpression(const std::vector<Lexeme>& lexemes, size_t startIdx, size_t endIdx, int lineNumber);
    void addError(int lineNumber, const std::string& errorMessage, const std::string& context = "");
    bool validateOperands(const std::vector<Lexeme>& lexemes, int lineNumber);
    bool checkLabelExists(const std::string& label, int lineNumber);
    bool checkVariableExists(const std::string& variable, int lineNumber);
    bool checkVariableRedefinition(const std::string& varName, int lineNumber);
    bool checkValidRegister(const std::string& reg, int lineNumber);

public:
    SecondPassListing(const FirstPassListing& firstPass);
    void generateListing(const std::string& inputFile, const std::string& outputFile);
    void printSymbolsTable();
    void printErrorSummary();
    int getErrorCount() const;
};
#endif