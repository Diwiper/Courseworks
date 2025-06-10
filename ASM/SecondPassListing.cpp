#include "SecondPassListing.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <stack>

SecondPassListing::SecondPassListing(const FirstPassListing& fp) : firstPass(fp) {
	initializeRegisters();
}


void SecondPassListing::initializeRegisters() {
	registers["AL"] = 0;
	registers["CL"] = 1;
	registers["DL"] = 2;
	registers["BL"] = 3;
	registers["AH"] = 4;
	registers["CH"] = 5;
	registers["DH"] = 6;
	registers["BH"] = 7;

	registers["AX"] = 0;
	registers["CX"] = 1;
	registers["DX"] = 2;
	registers["BX"] = 3;
	registers["SP"] = 4;
	registers["BP"] = 5;
	registers["SI"] = 6;
	registers["DI"] = 7;

	registers["EAX"] = 0;
	registers["ECX"] = 1;
	registers["EDX"] = 2;
	registers["EBX"] = 3;
	registers["ESP"] = 4;
	registers["EBP"] = 5;
	registers["ESI"] = 6;
	registers["EDI"] = 7;

	registers["ES"] = 0;
	registers["CS"] = 1;
	registers["SS"] = 2;
	registers["DS"] = 3;
	registers["FS"] = 4;
	registers["GS"] = 5;
}

void SecondPassListing::addError(int lineNumber, const std::string& errorMessage, const std::string& context) {
	Error error;
	error.lineNumber = lineNumber;
	error.errorMessage = errorMessage;
	error.context = context;
	errors.push_back(error);
}

int SecondPassListing::getErrorCount() const {
	return errors.size();
}

void SecondPassListing::printErrorSummary() {
	outputFile << "Errors list:" << std::endl;

	if (errors.empty()) {
		outputFile << "No errors found." << std::endl;
	}
	else {
		std::sort(errors.begin(), errors.end(), [](const Error& a, const Error& b) {
			return a.lineNumber < b.lineNumber;
			});

		outputFile << std::dec;

		for (const auto& error : errors) {
			outputFile << error.lineNumber << " " << error.errorMessage << error.context << std::endl;
		}
		outputFile << std::endl;
		outputFile << "Total errors: " << errors.size() << std::endl;
	}
}

bool SecondPassListing::checkLabelExists(const std::string& label, int lineNumber) {
	std::string upperLabel = toUpperLatin(label);

	for (const auto& segment : firstPass.getSegments()) {
		for (const auto& existingLabel : segment.second.labels) {
			if (existingLabel.name == upperLabel) {
				return true;
			}
		}
	}

	addError(lineNumber, "Error: Identifier '" + label + "'not defined");
	return false;
}

bool SecondPassListing::checkVariableExists(const std::string& variable, int lineNumber) {
	std::string upperVar = toUpperLatin(variable);
	bool found = false;

	const auto& segments = firstPass.getSegments();
	for (const auto& segment : segments) {
		for (const auto& var : segment.second.variables) {
			if (var.name == upperVar) {
				found = true;
				break;
			}
		}
		if (found) break;
	}

	if (!found) {
		addError(lineNumber, "Error: undefined variable: ", variable);
		return false;
	}
	return true;
}

bool SecondPassListing::checkVariableRedefinition(const std::string& varName, int lineNumber) {
	std::string upperVarName = toUpperLatin(varName);

	const auto& segments = firstPass.getSegments();
	bool alreadyDefined = false;
	std::string existingSegment;

	for (const auto& segment : segments) {
		for (const auto& var : segment.second.variables) {

			if (var.name == upperVarName && var.offset < currentAddress) {
				alreadyDefined = true;
				existingSegment = segment.first;
				break;
			}
		}
		if (alreadyDefined) break;
	}

	if (alreadyDefined) {
		addError(lineNumber, "Error: variable '" + varName + "' already defined in segment '" + existingSegment + "'", varName);
		return false;
	}

	for (const auto& segment : segments) {
		for (const auto& label : segment.second.labels) {
			if (label.name == upperVarName) {
				addError(lineNumber, "Error: variable name '" + varName + "' conflicts with a label in segment '" + segment.first + "'", varName);
				return false;
			}
		}
	}

	return true;
}


bool SecondPassListing::checkValidRegister(const std::string& reg, int lineNumber) {
	std::string upperReg = toUpperLatin(reg);
	if (registers.find(upperReg) == registers.end()) {
		addError(lineNumber, "Invalid register", reg);
		return false;
	}
	return true;
}

bool SecondPassListing::validateOperands(const std::vector<Lexeme>& lexemes, int lineNumber) {
	if (lexemes.empty()) return true;

	std::string instruction = toUpperLatin(lexemes[0].value);

	if (lexemes.size() >= 4) {
		if ((lexemes[1].type == "reg8" && (lexemes[3].type == "reg16" || lexemes[3].type == "reg32")) ||
			(lexemes[1].type == "reg16" && (lexemes[3].type == "reg8" || lexemes[3].type == "reg32")) ||
			(lexemes[1].type == "reg32" && (lexemes[3].type == "reg8" || lexemes[3].type == "reg16"))) {

			addError(lineNumber, "Mismatched register sizes", instruction + " " +
				lexemes[1].value + ", " + lexemes[3].value);
			return false;
		}
	}

	if (instruction == "JMP" || instruction == "JNZ") {
		if (lexemes.size() >= 2) {
			return checkLabelExists(lexemes[1].value, lineNumber);
		}
	}
	else if (instruction == "PUSH") {
		if (lexemes.size() >= 2 && lexemes[1].type == "identifier") {
			return checkVariableExists(lexemes[1].value, lineNumber);
		}
	}
	else if (instruction == "LEA") {
		if (lexemes.size() >= 2) {
			if (!checkValidRegister(lexemes[1].value, lineNumber)) {
				return false;
			}

			std::string memOperand;
			for (size_t i = 3; i < lexemes.size(); i++) {
				if (lexemes[i].type != "symbol" || lexemes[i].value != ",") {
					memOperand += lexemes[i].value;
				}
			}

			if (memOperand.find('[') != std::string::npos) {
				std::string baseVar = memOperand.substr(0, memOperand.find('['));
				std::string indexReg = memOperand.substr(memOperand.find('[') + 1,
					memOperand.find(']') - memOperand.find('[') - 1);

				if (!checkVariableExists(baseVar, lineNumber)) {
					return false;
				}

				if (!checkValidRegister(indexReg, lineNumber)) {
					return false;
				}
			}
			else {
				return checkVariableExists(memOperand, lineNumber);
			}
		}
	}

	return true;
}

void SecondPassListing::generateListing(const std::string& inputFile, const std::string& outputFile) {
	std::ifstream inFile(inputFile);
	this->outputFile.open(outputFile);

	if (!inFile.is_open() || !this->outputFile.is_open()) {
		std::cerr << "Error opening files!" << std::endl;
		return;
	}
	this->outputFile << "Denys Lashkov KV-33" << std::endl;
	this->outputFile << "Listing | Second view" << std::endl;
	this->outputFile << std::setw(48) << std::right << " " << std::endl;

	std::string line;
	int lineNumber = 1;

	while (std::getline(inFile, line)) {
		this->outputFile << lineNumber << "  ";
		processLine(line, lineNumber++);

	}

	printSymbolsTable();
	printErrorSummary();

	inFile.close();
	this->outputFile.close();
}

void SecondPassListing::processLine(const std::string& line, int lineNumber) {
	if (line.empty() || isWhitespaceOnly(line)) {
		outputFile << std::endl;
		return;
	}

	std::vector<Lexeme> lexemes = analyzeLine(const_cast<std::string&>(line));

	if (lexemes.empty()) {
		outputFile << std::endl;
		return;
	}

	if (lexemes.size() >= 1 && lexemes[0].type == "identifier" &&
		lexemes[0].value.back() == ':')
	{
		std::vector<Lexeme> labelLexemes = lexemes;
		labelLexemes[0].value = labelLexemes[0].value.substr(0, labelLexemes[0].value.size() - 1);
		processLabel(labelLexemes, lineNumber);
		return;
	}

	if (lexemes.size() >= 2 && lexemes[0].type == "identifier" &&
		(lexemes[1].type == "symbol" || lexemes[1].type == "operator" || lexemes[1].type == "delimiter") &&
		lexemes[1].value == ":") 
	{
		processLabel(lexemes, lineNumber);
		return;
	}

	if (lexemes.size() >= 2 && lexemes[1].type == "segment_start") {
		processSegmentStart(lexemes);
	}
	else if (lexemes.size() >= 2 && lexemes[1].type == "segment_ends") {
		processSegmentEnd(lexemes);
	}
	else if (lexemes.size() >= 1 && lexemes[0].type == "program_end") {
		outputFile << formatAddress(currentAddress) << std::setw(34) << " " << "END" << std::endl;
	}
	else if (lexemes.size() >= 2 && lexemes[1].type == "var-size") {
		processVariable(lexemes, lineNumber);
	}
	else if (lexemes.size() >= 1 && lexemes[0].type == "instruction") {
		validateOperands(lexemes, lineNumber);
		processInstruction(lexemes, lineNumber);
	}
	else {
		outputFile << std::setw(48) << " " << line << std::endl;
	}
}

void SecondPassListing::processSegmentStart(const std::vector<Lexeme>& lexemes) {
	currentSegment = toUpperLatin(lexemes[0].value);
	currentAddress = 0;
	outputFile << std::setw(43) << " " << currentSegment << " segment" << std::endl;
}

void SecondPassListing::processSegmentEnd(const std::vector<Lexeme>& lexemes) {
	outputFile << std::setw(43) << " " << currentSegment << " ends" << std::endl;
}

void SecondPassListing::processVariable(const std::vector<Lexeme>& lexemes, int lineNumber) {
	if (lexemes.size() < 2) {
		addError(lineNumber, "Invalid variable definition", "");
		return;
	}

	std::string varName = toUpperLatin(lexemes[0].value);
	std::string varType = toUpperLatin(lexemes[1].value);
	checkVariableRedefinition(varName, lineNumber);

	if (varType != "DB" && varType != "DW" && varType != "DD") {
		addError(lineNumber, "Invalid variable type", varType);
	}
	if (varType.size() < 2) {
		addError(lineNumber, "Invalid variable type", varType);
	}

	std::vector<unsigned char> dataBytes;
	try {
		dataBytes = calculateVariableData(varType, lexemes, lineNumber);
	}
	catch (const std::exception& e) {
		addError(lineNumber, "Error calculating variable data", e.what());
	}

	std::string addrStr = formatAddress(currentAddress);
	std::string machineCodeStr = formatMachineCode(dataBytes, false, 30);

	outputFile << addrStr << " " << machineCodeStr << varName << " " << varType << " ";

	for (size_t i = 2; i < lexemes.size(); i++)
		outputFile << lexemes[i].value;

	outputFile << std::endl;

	currentAddress += dataBytes.size();

}

void SecondPassListing::processLabel(const std::vector<Lexeme>& lexemes, int lineNumber) {
	std::string labelName = toUpperLatin(lexemes[0].value);

	if (labelAddresses.find(labelName) != labelAddresses.end()) {
		addError(lineNumber, "Duplicate label definition", labelName);
	}

	labelAddresses[labelName] = currentAddress;

	std::string addrStr = formatAddress(currentAddress);

	outputFile << addrStr << "    " << std::setw(26) << " " << labelName << ":" << std::endl;
}

void SecondPassListing::processInstruction(const std::vector<Lexeme>& lexemes, int lineNumber) {
	std::string instruction = toUpperLatin(lexemes[0].value);

	MachineCode machineCode = generateMachineCode(lexemes);
	bool need_ref = false;

	if (instruction == "PUSH" || instruction == "CMP" || instruction == "LEA")
		need_ref = true;

	std::string addrStr = formatAddress(currentAddress);
	std::string machineCodeStr = formatMachineCode(machineCode.bytes, need_ref, 30);

	outputFile << addrStr << "    ";
	if (!machineCode.machineCodePrefix.empty()) {
		outputFile << machineCode.machineCodePrefix;
	}
	outputFile << machineCodeStr;

	outputFile << machineCode.opcode;
	if (!machineCode.operands.empty()) {
		outputFile << " " << machineCode.operands;
	}
	outputFile << std::endl;

	int prefixBytes = 0;
	if (!machineCode.machineCodePrefix.empty()) {

		std::string prefix = machineCode.machineCodePrefix;
		if (prefix.find("66|") != std::string::npos) {
			prefixBytes += 1;
		}

		if (prefix.find("67|") != std::string::npos) {
			prefixBytes += 1;
		}

		if (prefix.find(":") != std::string::npos) {
			prefixBytes += 1;
		}
	}
	currentAddress += machineCode.bytes.size();
	currentAddress += prefixBytes;
	if (instruction == "CMP")
		currentAddress--;
}

MachineCode SecondPassListing::generateMachineCode(const std::vector<Lexeme>& lexemes) {
	std::string instruction = toUpperLatin(lexemes[0].value);

	if (instruction == "PUSH") {
		return generatePushInstruction(lexemes);
	}
	else if (instruction == "BT") {
		return generateBtInstruction(lexemes);
	}
	else if (instruction == "LEA") {
		return generateLeaInstruction(lexemes);
	}
	else if (instruction == "OUT") {
		return generateOutInstruction(lexemes);
	}
	else if (instruction == "CMP") {
		return generateCmpInstruction(lexemes);
	}
	else if (instruction == "JMP" || instruction == "JNZ") {
		return generateJmpInstruction(lexemes);
	}
	else if (instruction == "CLI") {
		return generateCliInstruction(lexemes);
	}

	return MachineCode{ std::vector<unsigned char>{0x90}, instruction, "" };
}

MachineCode SecondPassListing::generateCliInstruction(const std::vector<Lexeme>& lexemes) {
	std::string instruction = toUpperLatin(lexemes[0].value);
	std::vector<unsigned char> bytes = { 0xFA };

	return MachineCode{ bytes, instruction, "" };
}

MachineCode SecondPassListing::generatePushInstruction(const std::vector<Lexeme>& lexemes) {
	std::string instruction = "PUSH";
	std::string operandStr;
	std::vector<unsigned char> bytes;
	size_t i = 1;
	std::string machineCodePrefix = "";

	int segOverride = -1;

	if (i < lexemes.size() && lexemes[i].type == "seg_reg") {
		std::string segRegName = toUpperLatin(lexemes[i].value);
		segOverride = getSegmentRegisterPrefix(segRegName);
		if (segOverride >= 0 && segOverride != 0x3E) {
			unsigned char segPrefix = segOverride;

			std::stringstream ss;
			ss << std::hex << static_cast<int>(segPrefix) << ": ";
			machineCodePrefix = ss.str();
			operandStr += segRegName + ":";
			i += 2;
		}
		else
		{
			operandStr += segRegName + ":";
			i += 2;
		}
	}

	std::string operand;
	if (i < lexemes.size()) {
		operand = toUpperLatin(lexemes[i].value);
		operandStr += operand;

		bool isInCodeSegment = false;
		const auto& segments = firstPass.getSegments();
		for (const auto& segment : segments) {
			if (segment.first == "CODE" || segment.first == "code") {
				for (const auto& var : segment.second.variables) {
					if (var.name == operand) {
						isInCodeSegment = true;
						break;
					}
				}
			}
		}

		if (isInCodeSegment && segOverride == -1)
		{
			std::stringstream ss;
			ss << std::hex << static_cast<int>(getSegmentRegisterPrefix("CS")) << ": ";
			machineCodePrefix = ss.str();
		}

		bytes.push_back(0xFF);
		bytes.push_back(0x36);

		bool is32BitVar = false;
		int offset = 0;
		for (const auto& segment : segments) {
			for (const auto& var : segment.second.variables) {
				if (var.name == operand) {
					offset = var.offset;
					if (var.type == "DD") {
						is32BitVar = true;
					}
					break;
				}
			}
		}

		bytes.push_back((offset >> 8) & 0xFF);
		bytes.push_back(offset & 0xFF);


		if (is32BitVar) {
			if (!machineCodePrefix.empty()) {
				machineCodePrefix = "66| " + machineCodePrefix;
			}
			else {
				machineCodePrefix = "66| ";
			}
		}
	}

	return MachineCode{ bytes, instruction, operandStr, machineCodePrefix };
}

MachineCode SecondPassListing::generateBtInstruction(const std::vector<Lexeme>& lexemes) {
	std::string instruction = "BT";
	std::vector<unsigned char> bytes;
	std::string operandStr;
	std::string machineCodePrefix = "";

	if (lexemes.size() >= 4) {
		std::string op1 = toUpperLatin(lexemes[1].value);
		std::string op2 = toUpperLatin(lexemes[3].value);

		operandStr = op1 + ", " + op2;

		bool is32Bit = (lexemes[1].type == "reg32" || lexemes[3].type == "reg32");

		if (is32Bit) {
			machineCodePrefix = "66| ";
		}

		bytes.push_back(0x0F);
		bytes.push_back(0xA3);

		int reg1Code = getRegisterCode(op1);
		int reg2Code = getRegisterCode(op2);

		bytes.push_back(0xC0 | (reg2Code << 3) | reg1Code);
	}

	return MachineCode{ bytes, instruction, operandStr, machineCodePrefix };
}

MachineCode SecondPassListing::generateLeaInstruction(const std::vector<Lexeme>& lexemes) {
	std::string instruction = "LEA";
	std::vector<unsigned char> bytes;
	std::string operandStr;
	std::string machineCodePrefix = "";

	if (lexemes.size() >= 3) {
		std::string regDest = toUpperLatin(lexemes[1].value);
		int regCode = getRegisterCode(regDest);

		bool is32BitReg = (lexemes[1].type == "reg32");

		if (is32BitReg) {
			machineCodePrefix = "66| ";
		}

		operandStr = regDest + ", ";

		std::string memOperand;
		for (size_t i = 3; i < lexemes.size(); i++) {
			if (lexemes[i].type != "symbol" || lexemes[i].value != ",") {
				memOperand += lexemes[i].value;
				operandStr += lexemes[i].value;
			}
		}

		if (memOperand.find('[') != std::string::npos) {
			bytes.push_back(0x8D);
			std::string baseVar = memOperand.substr(0, memOperand.find('['));
			std::string indexReg = memOperand.substr(memOperand.find('[') + 1,
				memOperand.find(']') - memOperand.find('[') - 1);

			bool is32BitIndex = (indexReg.find("E") == 0);

			if (is32BitIndex) {
				machineCodePrefix += "67| ";
			}

			int rmField = 0;
			std::string upperIndexReg = toUpperLatin(indexReg);
			if (upperIndexReg == "SI") rmField = 4;    
			else if (upperIndexReg == "DI") rmField = 5; 
			else if (upperIndexReg == "BP") rmField = 6; 
			else if (upperIndexReg == "BX") rmField = 7; 
			else rmField = getRegisterCode(toUpperLatin(indexReg));

			int mod = 0x02; 

			unsigned char modRM = (mod << 6) | (regCode << 3) | rmField;
			bytes.push_back(modRM);

			const auto& segments = firstPass.getSegments();
			for (const auto& segment : segments) {
				for (const auto& var : segment.second.variables) {
					if (var.name == toUpperLatin(baseVar)) {
						int offset = var.offset;
						if (is32BitIndex) {
							bytes.push_back((offset >> 24) & 0xFF);
							bytes.push_back((offset >> 16) & 0xFF);
						}
						bytes.push_back((offset >> 8) & 0xFF);
						bytes.push_back(offset & 0xFF);

						break;
					}
				}
			}
		}
		else {
			bytes.push_back(0xBB | (regCode << 3));

			const auto& segments = firstPass.getSegments();
			for (const auto& segment : segments) {
				for (const auto& var : segment.second.variables) {
					if (var.name == toUpperLatin(memOperand)) {
						int offset = var.offset;
						bytes.push_back((offset >> 8) & 0xFF);
						bytes.push_back(offset & 0xFF);

						if (is32BitReg) {
							bytes.push_back((offset >> 24) & 0xFF);
							bytes.push_back((offset >> 16) & 0xFF);
						}
						break;
					}
				}
			}
		}
	}

	return MachineCode{ bytes, instruction, operandStr, machineCodePrefix };
}

MachineCode SecondPassListing::generateOutInstruction(const std::vector<Lexeme>& lexemes) {
	std::string instruction = "OUT";
	std::vector<unsigned char> bytes;
	std::string operandStr;
	std::string machineCodePrefix = "";

	if (lexemes.size() >= 4) {
		std::string port = toUpperLatin(lexemes[1].value);
		std::string reg = toUpperLatin(lexemes[3].value);

		operandStr = port + ", " + reg;

		bool is32BitReg = (lexemes[3].type == "reg32");

		if (is32BitReg) {
			machineCodePrefix = "66| ";
		}

		if (lexemes[1].type == "imm_hex" || lexemes[1].type == "imm_dec" || lexemes[1].type == "imm_bin") {
			long portValue = getImmediateValue(port);

			if (lexemes[3].type == "reg8") {
				bytes.push_back(0xE6);
			}
			else {
				bytes.push_back(0xE7);
			}

			bytes.push_back(portValue & 0xFF);
		}
		else if (port == "DX") {
			if (lexemes[3].type == "reg8") {
				bytes.push_back(0xEE);
			}
			else {
				bytes.push_back(0xEF);
			}
		}
	}

	return MachineCode{ bytes, instruction, operandStr, machineCodePrefix };
}

MachineCode SecondPassListing::generateCmpInstruction(const std::vector<Lexeme>& lexemes) {
	std::string instruction = "CMP";
	std::vector<unsigned char> bytes;
	std::string operandStr;
	std::string machineCodePrefix = "";

	if (lexemes.size() >= 4) {
		std::string op1 = toUpperLatin(lexemes[1].value);
		std::string op2;

		bool isExpression = false;
		for (size_t i = 3; i < lexemes.size(); i++) {
			if (i > 3 && lexemes[i].type == "symbol" &&
				(lexemes[i].value == "+" || lexemes[i].value == "-" ||
					lexemes[i].value == "*" || lexemes[i].value == "/" ||
					lexemes[i].value == "(" || lexemes[i].value == ")")) {
				isExpression = true;
				break;
			}
		}

		if (isExpression) {
			op2 = "";
			for (size_t i = 3; i < lexemes.size(); i++) {
				op2 += lexemes[i].value;
			}
		}
		else if (lexemes[3].type == "imm_dec" || lexemes[3].type == "imm_hex" || lexemes[3].type == "imm_bin") {
			op2 = toUpperLatin(lexemes[3].value);
		}
		else {
			for (size_t i = 3; i < lexemes.size(); i++) {
				if (i > 3) op2 += " ";
				op2 += lexemes[i].value;
			}
		}

		operandStr = op1 + ", " + op2;

		if (lexemes[1].type == "identifier" &&
			(lexemes[3].type == "imm_dec" || lexemes[3].type == "imm_hex" || lexemes[3].type == "imm_bin" ||
				(lexemes.size() > 4 && (lexemes[4].value == "+" || lexemes[4].value == "*" || lexemes[4].value == "-" || lexemes[4].value == "/")))) {

			long immValue;
			if ((lexemes[3].type == "imm_dec" || lexemes[3].type == "imm_hex" || lexemes[3].type == "imm_bin") &&
				(lexemes.size() > 4 && (lexemes[4].value != "+" && lexemes[4].value != "*" && lexemes[4].value != "-" && lexemes[4].value != "/")))
			{
				immValue = getImmediateValue(lexemes[3].value);
			}
			else {
				immValue = evaluateExpression(lexemes, 3, lexemes.size() - 1, 2);
			}

			int varSize = 1;
			const auto& segments = firstPass.getSegments();
			for (const auto& segment : segments) {
				for (const auto& var : segment.second.variables) {
					if (var.name == op1) {
						if (var.type == "DW") varSize = 2;
						else if (var.type == "DD") varSize = 4;
						break;
					}
				}
			}

			if (varSize == 4) {
				machineCodePrefix = "66| ";
			}

			bool useSmallForm = (immValue >= -128 && immValue <= 127);

			if (varSize == 1) {
				bytes.push_back(0x80);
			}
			else if (useSmallForm && (varSize == 2 || varSize == 4)) {
				bytes.push_back(0x83);
			}
			else {
				bytes.push_back(0x81);
			}

			bytes.push_back(0x3E);

			for (const auto& segment : segments) {
				for (const auto& var : segment.second.variables) {
					if (var.name == op1) {
						int offset = var.offset;
						bytes.push_back((offset >> 8) & 0xFF);
						bytes.push_back(offset & 0xFF);
						break;
					}
				}
			}

			if (varSize == 1 || useSmallForm) {
				bytes.push_back(0x72);
				bytes.push_back(immValue & 0xFF);
			}
			else if (varSize == 2) {
				bytes.push_back(0x72);
				bytes.push_back((immValue >> 8) & 0xFF);
				bytes.push_back(immValue & 0xFF);

			}
			else {
				bytes.push_back(0x72);
				bytes.push_back((immValue >> 24) & 0xFF);
				bytes.push_back((immValue >> 16) & 0xFF);
				bytes.push_back((immValue >> 8) & 0xFF);
				bytes.push_back(immValue & 0xFF);
			}
		}
		else if ((lexemes[1].type == "reg8" || lexemes[1].type == "reg16" || lexemes[1].type == "reg32") &&
			(lexemes[3].type == "reg8" || lexemes[3].type == "reg16" || lexemes[3].type == "reg32")) {

			int reg1Code = getRegisterCode(op1);
			int reg2Code = getRegisterCode(op2);

			if (lexemes[1].type == "reg32" || lexemes[3].type == "reg32") {
				machineCodePrefix = "66| ";
			}

			if (lexemes[1].type == "reg8") {
				bytes.push_back(0x3A);
			}
			else {
				bytes.push_back(0x3B);
			}

			bytes.push_back(0xC0 | (reg1Code << 3) | reg2Code);
		}
		else if ((lexemes[1].type == "reg8" || lexemes[1].type == "reg16" || lexemes[1].type == "reg32") &&
			(lexemes[3].type == "imm_dec" || lexemes[3].type == "imm_hex" || lexemes[3].type == "imm_bin")) {

			int regCode = getRegisterCode(op1);
			long immValue = getImmediateValue(op2);

			bool useSmallForm = (immValue >= -128 && immValue <= 127) &&
				(lexemes[1].type == "reg16" || lexemes[1].type == "reg32");

			if (lexemes[1].type == "reg32") {
				machineCodePrefix = "66| ";
			}

			if (regCode == 0) {
				if (lexemes[1].type == "reg8") {
					bytes.push_back(0x3C);
					bytes.push_back(immValue & 0xFF);
				}
				else {
					bytes.push_back(0x3D);
					bytes.push_back((immValue >> 8) & 0xFF);
					bytes.push_back(immValue & 0xFF);


					if (lexemes[1].type == "reg32") {
						bytes.push_back((immValue >> 24) & 0xFF);
						bytes.push_back((immValue >> 16) & 0xFF);
					}
				}
			}
			else {
				if (lexemes[1].type == "reg8") {
					bytes.push_back(0x80);
				}
				else if (useSmallForm) {
					bytes.push_back(0x83);
				}
				else {
					bytes.push_back(0x81);
				}

				bytes.push_back(0xF8 | regCode);

				bytes.push_back(immValue & 0xFF);

				if (lexemes[1].type != "reg8" && !useSmallForm) {
					bytes.push_back((immValue >> 8) & 0xFF);

					if (lexemes[1].type == "reg32") {
						bytes.push_back((immValue >> 24) & 0xFF);
						bytes.push_back((immValue >> 16) & 0xFF);
					}
				}
			}
		}
	}

	return MachineCode{ bytes, instruction, operandStr, machineCodePrefix };
}

MachineCode SecondPassListing::generateJmpInstruction(const std::vector<Lexeme>& lexemes)
{
	std::string instruction = toUpperLatin(lexemes[0].value);
	std::vector<unsigned char> bytes;
	std::string operandStr;

	if (lexemes.size() >= 2) {
		std::string targetLabel = toUpperLatin(lexemes[1].value);
		operandStr = targetLabel;
		int targetAddress = 0, displacement = 0;
		for (const auto& segment : firstPass.getSegments())
		{
			for (const auto& existingLabel : segment.second.labels) {
				if (existingLabel.name == targetLabel) {
					targetAddress = existingLabel.offset;
					displacement = targetAddress - (currentAddress + 2);
				}
			}
		}

		if (instruction == "JMP") {

			if (displacement >= -128 && displacement < 0) {
				bytes.push_back(0xEB);
				bytes.push_back(displacement & 0xFF);
			}
			else if (displacement <= 127 && displacement >= 0)
			{
				bytes.push_back(0xEB);
				bytes.push_back(displacement & 0xFF);
				bytes.push_back(0x90);
			}
			else
			{
				bytes.push_back(0xE9);
				displacement = targetAddress - (currentAddress + 3);
				bytes.push_back((displacement >> 8) & 0xFF);
				bytes.push_back(displacement & 0xFF);
			}
		}
		else
			if (instruction == "JNZ") {

				if (displacement >= -128 && displacement < 0) {
					bytes.push_back(0x75);
					bytes.push_back(displacement & 0xFF);
				}
				else if (displacement <= 127 && displacement >= 0)
				{
					bytes.push_back(0x75);
					bytes.push_back(displacement & 0xFF);
					bytes.push_back(0x90);
					bytes.push_back(0x90);
				}
				else
				{
					bytes.push_back(0x0F);
					bytes.push_back(0x85);
					displacement = targetAddress - (currentAddress + 4);
					bytes.push_back((displacement >> 8) & 0xFF);
					bytes.push_back(displacement & 0xFF);

				}

			}
	}
	return MachineCode{ bytes, instruction, operandStr };
}

std::vector<unsigned char> SecondPassListing::calculateVariableData(const std::string& type, const std::vector<Lexeme>& lexemes, int lineNumber) {
	std::vector<unsigned char> dataBytes;

	size_t startIdx = 2;

	if (startIdx < lexemes.size() && lexemes[startIdx].value.front() == '"') {
		std::string str = lexemes[startIdx].value;
		str = str.substr(1, str.size() - 2);

		for (char c : str) {
			dataBytes.push_back(static_cast<unsigned char>(c));
		}

		return dataBytes;
	}

	if (startIdx < lexemes.size()) {
		bool isExpression = false;
		for (size_t i = startIdx; i < lexemes.size(); i++) {
			if (lexemes[i].type == "symbol" &&
				(lexemes[i].value == "+" || lexemes[i].value == "-" ||
					lexemes[i].value == "*" || lexemes[i].value == "/" ||
					lexemes[i].value == "(" || lexemes[i].value == ")")) {
				isExpression = true;
				break;
			}
		}

		long value;
		try {
			if (isExpression) {
				value = evaluateExpression(lexemes, startIdx, lexemes.size() - 1, lineNumber);
			}
			else if (lexemes[startIdx].type == "imm_dec" || lexemes[startIdx].type == "imm_hex" ||
				lexemes[startIdx].type == "imm_bin") {
				value = getImmediateValue(lexemes[startIdx].value);
			}
			else {
				value = 0;
				addError(lineNumber, "Invalid operand for variable initialization", lexemes[startIdx].value);
			}

			if (type == "DB") {
				if (value < -128 || value > 255) {
					addError(lineNumber, "Value out of range for DB", std::to_string(value));
				}
				dataBytes.push_back(value & 0xFF);
			}
			else if (type == "DW") {
				if (value < -32768 || value > 65535) {
					addError(lineNumber, "Value out of range for DW", std::to_string(value));
				}
				dataBytes.push_back((value >> 8) & 0xFF);
				dataBytes.push_back(value & 0xFF);
			}
			else if (type == "DD") {
				dataBytes.push_back((value >> 24) & 0xFF);
				dataBytes.push_back((value >> 16) & 0xFF);
				dataBytes.push_back((value >> 8) & 0xFF);
				dataBytes.push_back(value & 0xFF);
			}
			else {
				addError(lineNumber, "Unknown variable type", type);
			}
		}
		catch (const std::exception& e) {
			addError(lineNumber, "Error evaluating expression", e.what());
		}
	}

	return dataBytes;
}

long SecondPassListing::evaluateExpression(const std::vector<Lexeme>& lexemes, size_t startIdx, size_t endIdx, int lineNumber) {
	if (startIdx == endIdx) {
		try {
			return getImmediateValue(lexemes[startIdx].value);
		}
		catch (const std::exception& e) {
			addError(lineNumber, "Invalid immediate value", lexemes[startIdx].value);
			return 0;
		}
	}

	if (lexemes[startIdx].value == "-" && startIdx + 1 <= endIdx) {
		try {
			return -getImmediateValue(lexemes[startIdx + 1].value);
		}
		catch (const std::exception& e) {
			addError(lineNumber, "Invalid immediate value", lexemes[startIdx + 1].value);
			return 0;
		}
	}

	auto getPrecedence = [](char op) -> int {
		if (op == '+' || op == '-') return 1;
		if (op == '*' || op == '/') return 2;
		return 0;
		};

	std::stack<long> values;
	std::stack<char> ops;
	bool expectValue = true;

	for (size_t i = startIdx; i <= endIdx; i++) {
		const Lexeme& lex = lexemes[i];

		if (lex.type == "symbol" && lex.value == ",") {
			continue;
		}

		if (lex.type == "imm_dec" || lex.type == "imm_hex" || lex.type == "imm_bin") {
			if (!expectValue) {
				addError(lineNumber, "Expression syntax error - expected operator", lex.value);
				return 0;
			}
			try {
				values.push(getImmediateValue(lex.value));
				expectValue = false;
			}
			catch (const std::exception& e) {
				addError(lineNumber, "Invalid immediate value", lex.value);
				return 0;
			}
		}
		else if (lex.type == "symbol" && lex.value == "(") {
			if (!expectValue) {
				addError(lineNumber, "Expression syntax error - unexpected '('", "");
				return 0;
			}
			ops.push('(');
			expectValue = true;
		}
		else if (lex.type == "symbol" && lex.value == ")") {
			if (expectValue) {
				addError(lineNumber, "Expression syntax error - unexpected ')'", "");
				return 0;
			}

			bool foundOpeningParen = false;
			while (!ops.empty() && ops.top() != '(') {
				if (values.size() < 2) {
					addError(lineNumber, "Expression syntax error - insufficient operands", "");
					return 0;
				}

				int val2 = values.top(); values.pop();
				int val1 = values.top(); values.pop();
				char op = ops.top(); ops.pop();

				try {
					if (op == '+') values.push(val1 + val2);
					else if (op == '-') values.push(val1 - val2);
					else if (op == '*') values.push(val1 * val2);
					else if (op == '/') {
						if (val2 == 0) {
							addError(lineNumber, "Division by zero", "");
							return 0;
						}
						values.push(val1 / val2);
					}
				}
				catch (const std::exception& e) {
					addError(lineNumber, "Arithmetic error", e.what());
					return 0;
				}
			}

			if (!ops.empty() && ops.top() == '(') {
				ops.pop();
				foundOpeningParen = true;
			}

			if (!foundOpeningParen) {
				addError(lineNumber, "Mismatched parentheses", "");
				return 0;
			}
			expectValue = false;
		}
		else if (lex.type == "symbol" && (lex.value == "+" || lex.value == "-" ||
			lex.value == "*" || lex.value == "/")) {
			if (expectValue) {
				if (lex.value == "-") {

					values.push(0);
				}
				else {
					addError(lineNumber, "Expression syntax error - unexpected operator", lex.value);
					return 0;
				}
			}

			char op = lex.value[0];

			try {
				while (!ops.empty() && ops.top() != '(' &&
					getPrecedence(ops.top()) >= getPrecedence(op)) {
					if (values.size() < 2) {
						addError(lineNumber, "Expression syntax error - insufficient operands", "");
						return 0;
					}

					int val2 = values.top(); values.pop();
					int val1 = values.top(); values.pop();
					char topOp = ops.top(); ops.pop();

					if (topOp == '+') values.push(val1 + val2);
					else if (topOp == '-') values.push(val1 - val2);
					else if (topOp == '*') values.push(val1 * val2);
					else if (topOp == '/') {
						if (val2 == 0) {
							addError(lineNumber, "Division by zero", "");
							return 0;
						}
						values.push(val1 / val2);
					}
				}
			}
			catch (const std::exception& e) {
				addError(lineNumber, "Arithmetic error", e.what());
				return 0;
			}

			ops.push(op);
			expectValue = true;
		}
		else {
			addError(lineNumber, "Invalid token in expression", lex.value);
			return 0;
		}
	}

	try {
		while (!ops.empty()) {
			if (values.size() < 2) {
				addError(lineNumber, "Expression syntax error - insufficient operands", "");
				return 0;
			}

			int val2 = values.top(); values.pop();
			int val1 = values.top(); values.pop();
			char op = ops.top(); ops.pop();

			if (op == '(') {
				addError(lineNumber, "Mismatched parentheses", "");
				return 0;
			}

			if (op == '+') values.push(val1 + val2);
			else if (op == '-') values.push(val1 - val2);
			else if (op == '*') values.push(val1 * val2);
			else if (op == '/') {
				if (val2 == 0) {
					addError(lineNumber, "Division by zero", "");
					return 0;
				}
				values.push(val1 / val2);
			}
		}
	}
	catch (const std::exception& e) {
		addError(lineNumber, "Arithmetic error", e.what());
		return 0;
	}

	if (values.empty()) {
		addError(lineNumber, "Empty expression", "");
		return 0;
	}

	if (values.size() > 1) {
		addError(lineNumber, "Invalid expression - multiple values remain", "");
	}

	return values.top();
}

std::string SecondPassListing::formatAddress(int address) {
	std::stringstream ss;
	ss << std::setw(4) << std::setfill('0') << std::hex << address;
	return ss.str();
}

std::string SecondPassListing::formatHexByte(unsigned char byte) {
	std::stringstream ss;
	ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
	return ss.str();
}

std::string SecondPassListing::formatMachineCode(const std::vector<unsigned char>& bytes, bool need_ref, int maxWidth) {
	if (bytes.empty()) {
		return std::string(maxWidth, ' ');
	}

	std::string result;
	for (size_t i = 0; i < bytes.size(); i++) {
		if (bytes[i] == 114 && need_ref) {
			if (!result.empty()) {
				result.pop_back();
			}
			result += "r ";
			need_ref = false;
		}
		else {
			result += formatHexByte(bytes[i]) + " ";
		}
	}

	if (need_ref && !result.empty()) {
		result.pop_back();
		result += "r";
	}

	int padding = maxWidth - static_cast<int>(result.length());
	if (padding > 0) {
		result.append(padding, ' ');
	}

	return result;
}

int SecondPassListing::getRegisterCode(const std::string& regName) {
	std::string upperReg = toUpperLatin(regName);
	if (registers.find(upperReg) != registers.end()) {
		return registers[upperReg];
	}
	return 0;
}

int SecondPassListing::getSegmentRegisterPrefix(const std::string& segRegName) {
	std::string upperReg = toUpperLatin(segRegName);
	if (upperReg == "ES") return 0x26;
	if (upperReg == "CS") return 0x2E;
	if (upperReg == "SS") return 0x36;
	if (upperReg == "DS") return 0x3E;
	if (upperReg == "FS") return 0x64;
	if (upperReg == "GS") return 0x65;
	return -1;
}

long SecondPassListing::getImmediateValue(const std::string& immValue) {
	std::string upperImm = toUpperLatin(immValue);

	try {
		if (upperImm.find('H') != std::string::npos) {
			std::string hexStr = upperImm.substr(0, upperImm.find('H'));

			if (!std::isdigit(hexStr[0])) {
				hexStr = "0" + hexStr;
			}
			return std::stoul(hexStr, nullptr, 16);
		}
		else if (upperImm.find('B') != std::string::npos) {

			std::string binaryStr = upperImm.substr(0, upperImm.find('B'));
			return std::stoul(binaryStr, nullptr, 2);
		}
		else if (upperImm.find('D') != std::string::npos) {
			std::string decStr = upperImm.substr(0, upperImm.find('D'));
			return std::stoul(decStr);
		}
		else {
			return std::stoul(upperImm);
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error parsing value: " << immValue << " - " << e.what() << std::endl;
		return 0;
	}
}

void SecondPassListing::printSymbolsTable() {
	outputFile << std::endl << std::endl;
	outputFile << "SYMBOLS TABLE:" << std::endl;

	outputFile << "VARIABLES:" << std::endl;
	const auto& segments = firstPass.getSegments();
	for (const auto& segment : segments) {
		for (const auto& var : segment.second.variables) {
			outputFile << "Name:" << std::setw(10) << std::left << var.name;
			outputFile << " Segment:" << std::setw(6) << std::left << segment.second.name;
			outputFile << " Offset: " << formatAddress(var.offset) << std::endl;
		}
	}

	outputFile << "LABELS:" << std::endl;
	for (const auto& segment : segments) {
		for (const auto& label : segment.second.labels) {
			outputFile << "Name:" << std::setw(10) << std::left << label.name;
			outputFile << " Segment:" << std::setw(6) << std::left << segment.second.name;
			outputFile << " Offset: " << formatAddress(label.offset) << std::endl;
		}
	}
}