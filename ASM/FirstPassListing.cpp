#include "FirstPassListing.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <regex>

FirstPassListing::FirstPassListing() : address(0) {}

void FirstPassListing::addError(int lineNumber, const std::string& message) {
    errors.push_back({ lineNumber, message });
}

std::string FirstPassListing::getVariableSegment(const std::string& varName) {
    for (const auto& segmentPair : segments) {
        for (const auto& var : segmentPair.second.variables) {
            if (var.name == varName) {
                return var.segment;
            }
        }
    }
    return "";
}

bool FirstPassListing::is32BitVariable(const std::string& varName) {
    if (variableSizes.find(varName) != variableSizes.end()) {
        return variableSizes[varName] == 4;
    }
    return false;
}

std::string FirstPassListing::extractVariableName(const std::vector<Lexeme>& lexemes, size_t startIndex) {
    if (startIndex < lexemes.size() && lexemes[startIndex].type == "identifier") {
        return lexemes[startIndex].value;
    }
    return "";
}

std::string FirstPassListing::analyzeOperandType(const std::vector<Lexeme>& lexemes, size_t startIndex) {
    if (startIndex >= lexemes.size()) return "";

    if (lexemes[startIndex].type == "reg32") return "reg32";
    if (lexemes[startIndex].type == "reg16") return "reg16";
    if (lexemes[startIndex].type == "reg8") return "reg8";
    if (lexemes[startIndex].type == "seg_reg") return "seg_reg";

    if (lexemes[startIndex].type == "identifier") {
        std::string varName = lexemes[startIndex].value;
        if (variableTypes.find(varName) != variableTypes.end()) {
            return variableTypes[varName]; // "DB", "DW", or "DD"
        }
    }

    return "unknown";
}

bool FirstPassListing::validateInstruction(const std::vector<Lexeme>& lexemes, int lineNumber) {
    if (lexemes.empty() || lexemes[0].type != "instruction") return true; // Not an instruction

    std::string mnemonic = lexemes[0].value;

    if (mnemonic == "CLI" && lexemes.size() > 1) {
        addError(lineNumber, "CLI instruction should not have operands");
        return false;
    }

    if (mnemonic == "JMP") {
        if (lexemes.size() <= 1) {

            addError(lineNumber, "JMP instruction requires a label operand");
            return false;
        }
        if (lexemes[1].type != "identifier") {
            addError(lineNumber, "JMP operand must be a label");
            return false;
        }

        bool labelFound = false;
        for (const auto& segmentPair : segments) {
            for (const auto& label : segmentPair.second.labels) {
                if (label.name == lexemes[1].value) {
                    labelFound = true;
                    break;
                }
            }
            if (labelFound) break;
        }
        if (!labelFound && !lexemes[1].value.empty()) {
            if(lineNumber)
            addError(lineNumber, "Warning: JMP target label '" + lexemes[1].value + "' not defined in the current pass");
            return false;
        }
    }

    if (mnemonic == "JNZ") {
        if (lexemes.size() <= 1) {

            addError(lineNumber, "JNZ instruction requires a label operand");
            return false;
        }
        if (lexemes[1].type != "identifier") {
            addError(lineNumber, "JNZ operand must be a label");
            return false;
        }

        bool labelFound = false;
        for (const auto& segmentPair : segments) {
            for (const auto& label : segmentPair.second.labels) {
                if (label.name == lexemes[1].value) {
                    labelFound = true;
                    break;
                }
            }
            if (labelFound) break;
        }
        if (!labelFound && !lexemes[1].value.empty()) {

            if (lineNumber)
                addError(lineNumber, "Warning: JNZ target label '" + lexemes[1].value + "' not defined in the current pass");
            return false;
        }
    }

    if (mnemonic == "PUSH") {
        if (lexemes.size() <= 1) {
            addError(lineNumber, "PUSH instruction requires an operand");
            return false;
        }
    }


    return true;
}

bool FirstPassListing::validateVariable(const std::vector<Lexeme>& lexemes, int lineNumber) {
    if (lexemes.size() < 3) return true; // Not a variable definition

    if (lexemes[0].type == "identifier" &&
        (lexemes[1].value == "DB" || lexemes[1].value == "DW" || lexemes[1].value == "DD")) {

        std::string varName = lexemes[0].value;
        for (const auto& segmentPair : segments) {
            for (const auto& var : segmentPair.second.variables) {
                if (var.name == varName && var.segment == currentSegment) {
                    addError(lineNumber, "Error: variable '" + varName + "' already defined in segment '" + currentSegment + "'");
                    return false;
                }
            }
        }

        if (lexemes.size() <= 2) {
            addError(lineNumber, "Variable '" + varName + "' defined without initializer");
            return false;
        }

        // Check if variable name conflicts with a label
        for (const auto& segmentPair : segments) {
            for (const auto& label : segmentPair.second.labels) {
                if (label.name == varName && label.segment == currentSegment) {
                    addError(lineNumber, "Variable name '" + varName + "' conflicts with a label in the same segment");
                    return false;
                }
            }
        }
    }

    return true;
}

bool FirstPassListing::validateSegment(const std::vector<Lexeme>& lexemes, int lineNumber) {
    if (lexemes.size() >= 2 && lexemes[1].type == "segment_start") {
        std::string segmentName = lexemes[0].value;

        if (segments.find(segmentName) != segments.end()) {
            addError(lineNumber, "Segment '" + segmentName + "' already defined");
            return false;
        }
    }

    if (lexemes.size() >= 2 && lexemes[1].type == "segment_ends") {
        std::string segmentName = lexemes[0].value;

        if (segmentName != currentSegment) {
            addError(lineNumber, "ENDS for segment '" + segmentName + "' does not match current segment '" + currentSegment + "'");
            return false;
        }
    }

    return true;
}

bool FirstPassListing::validateLabel(const std::vector<Lexeme>& lexemes, int lineNumber) {
    if (lexemes.size() >= 2 && lexemes[0].type == "identifier" && lexemes[1].value == ":") {
        std::string labelName = lexemes[0].value;

        for (const auto& label : segments[currentSegment].labels) {
            if (label.name == labelName) {
                addError(lineNumber, "Label '" + labelName + "' already defined in segment '" + currentSegment + "'");
                return false;
            }
        }

        for (const auto& var : segments[currentSegment].variables) {
            if (var.name == labelName) {
                addError(lineNumber, "Label name '" + labelName + "' conflicts with a variable in the same segment");
                return false;
            }
        }
    }

    return true;
}

int FirstPassListing::calculateSize(const std::vector<Lexeme>& lexemes) {

    if (lexemes.empty()) return 0;

    if (lexemes.size() >= 2 && lexemes[1].type == "var-size") {
        if (lexemes[1].value == "DB") return 1;
        if (lexemes[1].value == "DW") return 2;
        if (lexemes[1].value == "DD") return 4;
    }

    if (lexemes[0].type == "instruction") {
        std::string mnemonic = lexemes[0].value;


        if (mnemonic == "CLI") {
            return 1;
        }

        if (mnemonic == "JMP" && lexemes.size() > 1 && lexemes[1].type == "identifier") {
            std::string labelName = lexemes[1].value;
            bool labelFound = false;

            for (const auto& segmentPair : segments) {
                for (const auto& label : segmentPair.second.labels) {
                    if (label.name == labelName) {
                        labelFound = true;
                        int displacement = label.offset - address;

                        if (displacement >= -128)
                            return 2;
                        else 
                            return 3;  
                    }
                }
                if (labelFound) break;
            }

            return 3; 
        }

        if ((mnemonic == "JNZ") && lexemes.size() > 1 && lexemes[1].type == "identifier") {
            std::string labelName = lexemes[1].value;
            bool labelFound = false;

            for (const auto& segmentPair : segments) {
                for (const auto& label : segmentPair.second.labels) {
                    if (label.name == labelName) {
                        labelFound = true;
                        int displacement = label.offset - address;

                        if (displacement >= -128) 
                            return 2; 
                        else 
                            return 4;  
                    }
                }
                if (labelFound) break;
            }

            return 4; 
        }
      
        if (mnemonic == "PUSH") {
            bool hasSegRegPrefix = false;
            bool is32BitOperand = false;
            bool is16BitOperand = false;
            std::string variableName = "";
            std::string variableSegment = "";

            for (size_t i = 1; i < lexemes.size(); i++) {
                if (lexemes[i].type == "seg_reg") {
                    hasSegRegPrefix = true;
                }

                if (lexemes[i].type == "identifier") {
                    variableName = lexemes[i].value;

                    for (const auto& segmentPair : segments) {
                        for (const auto& var : segmentPair.second.variables) {
                            if (var.name == variableName) {
                                variableSegment = var.segment;

                                if (var.size == 4) {
                                    is32BitOperand = true;
                                }
                                else if (var.size == 2) {
                                    is16BitOperand = true;
                                }
                                break;
                            }
                        }
                        if (!variableSegment.empty()) break;
                    }

                    if (variableSegment.empty() && variableSizes.find(variableName) != variableSizes.end()) {
                        if (variableSizes[variableName] == 4) {
                            is32BitOperand = true;
                        }
                        else if (variableSizes[variableName] == 2) {
                            is16BitOperand = true;
                        }
                    }
                }
            }

            if (hasSegRegPrefix) 
            {
                if (lexemes[1].value == "DS") {
                    if (is16BitOperand) 
                    {
                        return 4;
                    }
                    if (is32BitOperand)
                        return 5;
                }
                else
                {
                    if (is16BitOperand)
                        return 5;
                    else
                        return 6;
                }
            }

            if (!variableSegment.empty()) {
                if (variableSegment == "DATA") {
                    if (is32BitOperand)
                        return 5;
                    if (is16BitOperand)
                        return 4;
                }
                else if (variableSegment == "CODE") {
                    if (is32BitOperand || is16BitOperand)
                        return 5;
                }
            }
            else
            {

                if (is32BitOperand || (is16BitOperand && currentSegment == "CODE"))
                    return 5;

                return 4;
            }

            return 4;
        }

        if (mnemonic == "BT") {
            bool is32BitOperand = false;

            for (size_t i = 1; i < lexemes.size(); i++) {
                if (lexemes[i].type == "reg32") {
                    is32BitOperand = true;
                    break;
                }
            }

            return is32BitOperand ? 4 : 3;
        }

        if (mnemonic == "LEA") {
            bool is32BitReg = false;
            bool hasIndexingReg = false;
            bool hasBaseReg = false;

            for (size_t i = 1; i < lexemes.size(); i++) {
                if (lexemes[i].type == "reg32") {
                    is32BitReg = true;
                }

                if ((i + 2 < lexemes.size()) &&
                    (lexemes[i].type == "identifier") &&
                    (lexemes[i + 1].value == "[") &&
                    ((lexemes[i + 2].type == "reg16") || (lexemes[i + 2].type == "reg32"))) {
                    hasIndexingReg = true;

                    if (lexemes[i + 2].type == "reg32") {
                        is32BitReg = true;
                    }
                }
            }

            if (is32BitReg) {
                if (hasIndexingReg) return 8;
                return 5;
            }

            if (hasIndexingReg) return 4;
            return 3;
        }

        if (mnemonic == "OUT") {
            for (size_t i = 1; i < lexemes.size(); i++) {
                if (lexemes[i].type == "reg32") {
                    return 3;
                }
                if (lexemes[i].type == "reg16") {
                    return 2; 
                }
                if (lexemes[i].type == "reg8") {
                    return 2;
                }
            }
        }

        if (mnemonic == "CMP") {
            bool is32BitOperand = false;
            bool is16BitOperand = false;
            bool is32BitImmediate = false;
            bool is8BitImmediate = false;

            if (lexemes.size() > 1 && lexemes[1].type == "identifier") 
            {

                if (variableTypes.find(lexemes[1].value) != variableTypes.end()) 
                {
                    if (variableTypes[lexemes[1].value] == "DD")
                    {
                        is32BitOperand = true;
                    }
                    if (variableTypes[lexemes[1].value] == "DW")
                    {
                        is16BitOperand = true;
                    }
                }
            }


            for (size_t i = 2; i < lexemes.size(); i++)
            {
                if (lexemes[i].type == "imm_hex" && lexemes[i].value.length() >= 4) {
                    is32BitImmediate = true;
                    break;
                }
                if (lexemes[i].type == "imm_bin" && lexemes[i].value.length() > 18) 
                {
                    is32BitImmediate = true;
                    break;
                }
                if (lexemes[i].type == "imm_dec" && std::stoi(lexemes[i].value) > 65535)
                {
                    is32BitImmediate = true;
                    break;
                }
                if (lexemes[i].type == "imm_hex" && std::stoi(lexemes[i].value, nullptr, 16) <= 0x7F)
                {
                    is8BitImmediate = true;
                    break;
                }
                if (lexemes[i].type == "imm_bin" && lexemes[i].value.length() < 9)
                {
                    is8BitImmediate = true;
                    break;
                }
                if (lexemes[i].type == "imm_dec" && (std::stoi(lexemes[i].value) < 128 && std::stoi(lexemes[i].value) > -127))
                {
                    is8BitImmediate = true;
                    break;
                }
            }
            if (is16BitOperand && is8BitImmediate) return 5;
            if (is32BitOperand&& is32BitImmediate) return 9;
            if (is32BitOperand || is16BitOperand) return 6;
            return 5;
        }
    }

    return 3; 
}

std::string FirstPassListing::formatAddress(int address) {
    std::stringstream ss;
    ss << std::hex << std::setw(4) << std::setfill('0') << address;
    return ss.str();
}

void FirstPassListing::processLine(const std::string& line, int lineNumber, std::ofstream& outputFile) {
    if (line.empty() || isWhitespaceOnly(line)) {
        outputFile << std::setw(4) << lineNumber << "  " << std::left << std::setw(10) << "" << line << std::endl;
        return;
    }

    std::vector<Lexeme> lexemes = analyzeLine(const_cast<std::string&>(line));
    if (lexemes.empty()) return;

    validateInstruction(lexemes, lineNumber);
    validateVariable(lexemes, lineNumber);
    validateSegment(lexemes, lineNumber);
    validateLabel(lexemes, lineNumber);

    if (lexemes.size() == 1 && lexemes[0].type == "program_end") {
        address -= 2;
        outputFile << std::setw(4) << lineNumber << "  " << formatAddress(address) << "    " << line << std::endl;
        return;
    }

    if (lexemes.size() >= 2 && lexemes[1].type == "segment_start") {
        currentSegment = lexemes[0].value;
        segments[currentSegment] = Segment{ currentSegment, 0 };
        address = 0;

        outputFile << std::setw(4) << lineNumber << "  " << std::left << std::setw(10) << "" << line << std::endl;
        return;
    }

    else if (lexemes.size() >= 2 && lexemes[1].type == "segment_ends") {
        if (lexemes[0].value != currentSegment) {
            addError(lineNumber, "Segment end mismatch: '" + lexemes[0].value +
                "' ENDS does not match current segment '" + currentSegment + "'");
        }

        outputFile << std::setw(4) << lineNumber << "  " << std::left << std::setw(10) << "" << line << std::endl;
        address += 2;
        return;
    }

    if (lexemes.size() >= 2 && lexemes[0].type == "identifier" && lexemes[1].value == ":") {
        Label label;
        label.name = lexemes[0].value;
        label.segment = currentSegment;
        label.offset = address;

        // Check for duplicate labels
        bool isDuplicate = false;
        for (const auto& existingLabel : segments[currentSegment].labels) {
            if (existingLabel.name == label.name) {
                addError(lineNumber, "Duplicate label '" + label.name + "' in segment '" + currentSegment + "'");
                isDuplicate = true;
                break;
            }
        }

        if (!isDuplicate) {
            segments[currentSegment].labels.push_back(label);
        }

        outputFile << std::setw(4) << lineNumber << "  " << formatAddress(address) << "    " << line << std::endl;
        return;
    }

    if (lexemes.size() >= 3 && lexemes[0].type == "identifier" &&
        (lexemes[1].value == "DB" || lexemes[1].value == "DW" || lexemes[1].value == "DD")) {

        Variable var;
        var.name = lexemes[0].value;
        var.segment = currentSegment;
        var.offset = address;
        var.type = lexemes[1].value;  

        if (lexemes[1].value == "DB") var.size = 1;
        else if (lexemes[1].value == "DW") var.size = 2;
        else if (lexemes[1].value == "DD") var.size = 4;

        bool isDuplicate = false;
        for (const auto& existingVar : segments[currentSegment].variables) {
            if (existingVar.name == var.name) {
                isDuplicate = true;
                break;
            }
        }

        if (!isDuplicate) {
            segments[currentSegment].variables.push_back(var);
            variableTypes[lexemes[0].value] = lexemes[1].value;
            variableSizes[lexemes[0].value] = var.size;

       
        }
 outputFile << std::setw(4) << lineNumber << "  " << formatAddress(address) << "    " << line << std::endl;
        address += var.size;
        return;
    }

    if (!lexemes.empty() && lexemes[0].type == "instruction") {
        for (size_t i = 1; i < lexemes.size(); i++) {
            if (lexemes[i].type == "identifier") {
                std::string identifier = lexemes[i].value;


                if (lexemes[i].type == "seg_reg" || lexemes[i].type == "reg32" ||
                    lexemes[i].type == "reg16" || lexemes[i].type == "reg8") {
                    continue;
                }
                bool isVariable = false;
                for (const auto& segmentPair : segments) {
                    for (const auto& var : segmentPair.second.variables) {
                        if (var.name == identifier) {
                            isVariable = true;
                            break;
                        }
                    }
                    if (isVariable) break;
                }

                bool isLabel = false;
                for (const auto& segmentPair : segments) {
                    for (const auto& label : segmentPair.second.labels) {
                        if (label.name == identifier) {
                            isLabel = true;
                            break;
                        }
                    }
                    if (isLabel) break;
                }
                std::string mnem = toUpperLatin(lexemes[0].value);
                if (!isVariable && mnem!="JMP" && mnem != "JNZ") {
                    addError(lineNumber, "Error: Identifier '" + identifier + "' used but not yet defined");
                }
            }
        }

        int size = calculateSize(lexemes);
        outputFile << std::setw(4) << lineNumber << "  " << formatAddress(address) << "    " << line << std::endl;
        address += size;
        return;
    }

    outputFile << std::setw(4) << lineNumber << "  " << formatAddress(address) << "    " << line << std::endl;
}

void FirstPassListing::generateListing(const std::string& inputFile, const std::string& outputFile) {
    std::ifstream inFile(inputFile);
    if (!inFile.is_open()) {
        std::cerr << "Failed to open input file: " << inputFile << std::endl;
        return;
    }

    std::ofstream outFile(outputFile);
    if (!outFile.is_open()) {
        std::cerr << "Failed to create output file: " << outputFile << std::endl;
        return;
    }

    outFile << "Denys Lashkov KV-33" << std::endl;
    outFile << "Listing | First view " << std::endl;

    std::string line;
    int lineNumber = 0;

    errors.clear();

    while (std::getline(inFile, line)) {
        lineNumber++;
        if (line.empty() || isWhitespaceOnly(line)) {
            outFile << std::setw(4) << lineNumber << "  " << std::left << std::setw(10) << ""
                << " " << std::endl;
        }
        else {
            processLine(line, lineNumber, outFile);
        }
    }

    outFile << std::endl;

    printVariablesList(outFile);
    outFile << std::endl;
    printLabelsList(outFile);
    outFile << std::endl;

    printErrorsList(outFile);

    inFile.close();
    outFile.close();
}

void FirstPassListing::printVariablesList(std::ofstream& outputFile) {
    outputFile << "Variables list:" << std::endl;

    for (const auto& segmentPair : segments) {
        for (const auto& var : segmentPair.second.variables) {
            outputFile << "Name:" << "\t" << var.name;
            outputFile << "\tSegment:" << var.segment;
            outputFile << "\tOffset:  " << std::right << std::hex << std::setw(4)
                << std::setfill('0') << var.offset << std::endl;
        }
    }
}

void FirstPassListing::printLabelsList(std::ofstream& outputFile) {
    outputFile << "Labels:" << std::endl;

    for (const auto& segmentPair : segments) {
        for (const auto& label : segmentPair.second.labels) {
            outputFile << "Name:" << "\t" << label.name;
            outputFile << "\tSegment:" << label.segment;
            outputFile << "\tOffset:  " << std::right << std::hex << std::setw(4)
                << std::setfill('0') << label.offset << std::endl;
        }
    }
}

void FirstPassListing::printErrorsList(std::ofstream& outputFile) {
    outputFile << "Errors list:" << std::endl;

    if (errors.empty()) {
        outputFile << "No errors found." << std::endl;
    }
    else {
        std::sort(errors.begin(), errors.end(), [](const AssemblyError& a, const AssemblyError& b) {
            return a.lineNumber < b.lineNumber;
            });

        outputFile << std::dec;

        for (const auto& error : errors) {
            outputFile << error.lineNumber << " " << error.message << std::endl;
        }
        outputFile << std::endl;
        outputFile << "Total errors: " << errors.size() << std::endl;
    }
}