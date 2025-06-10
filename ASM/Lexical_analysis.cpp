#include"Lexical_analysis.h"
#include <map>

bool isWhitespaceOnly(const std::string& str) 
{
    return str.find_first_not_of(" \t\r\n") == std::string::npos;
}

std::string toUpperLatin(const std::string& input)
{
    std::string result;
    for (char c : input) {
        if (c >= 'a' && c <= 'z') {
            result += c - ('a' - 'A');  
        }
        else {
            result += c;
        }
    }
    return result;
}

std::vector<Lexeme> analyzeLine(std::string& line) 
{
    line = toUpperLatin(line);  // Convert string to uppercase

    std::vector<Lexeme> lexemes;
    std::regex generalRegisters32(R"(EAX|EBX|ECX|EDX|ESI|EDI|EBP|ESP)");
    std::regex generalRegisters16(R"(AX|BX|CX|DX|SI|DI|BP|SP)");
    std::regex generalRegisters8(R"(AH|AL|BH|BL|CH|CL|DH|DL)");
    std::regex segmentRegisters(R"(CS|DS|ES|FS|GS|SS)");
    std::regex identifier(R"([A-Z_][A-Z0-9_]*)");
    std::regex binaryConst(R"([01]+B)");
    std::regex hexConst(R"([0-9A-F]+H)");
    std::regex decimalConst(R"([0-9]+D?)");
    std::regex directive(R"(DB|DW|DD|SEGMENT|ENDS|END)");
    std::regex instruction(R"(CLI|PUSH|BT|LEA|OUT|CMP|JMP|JNZ)");
    std::regex symbol(R"([,:\-+*/()\[\]])");
    std::regex textConst(R"(".*")");


    std::regex tokenRegex(R"([,:+\-*/()\[\]]|".*"|[A-Z_][A-Z0-9_]*|[01]+B|[0-9A-F]+H|[0-9]+D?|[\S]+)");

    std::sregex_iterator iter(line.begin(), line.end(), tokenRegex);
    std::sregex_iterator end;

    int lexemeIndex = 1;

    for (; iter != end; ++iter) 
    {
        std::string token = iter->str();
        if (token.empty()) continue;

        Lexeme lexeme;
        lexeme.value = token;
        lexeme.length = token.size();
        lexeme.index = lexemeIndex++;

        std::regex fullHexPattern(R"(0[0-9A-F]+H)");
        if (regex_match(token, fullHexPattern)) 
        {
            lexeme.type = "imm_hex";
            lexemes.push_back(lexeme);
            continue;
        }

        if (regex_match(token, instruction)) {
            lexeme.type = "instruction";
        }
        else if (token == "SEGMENT") {
            lexeme.type = "segment_start";
        }
        else if (token == "ENDS") {
            lexeme.type = "segment_ends";
        }
        else if (token == "END") {
            lexeme.type = "program_end";
        }
        else if (token == "DB" || token == "DW" || token == "DD") {
            lexeme.type = "var-size";
        }
        else if (regex_match(token, generalRegisters32)) {
            lexeme.type = "reg32";
        }
        else if (regex_match(token, generalRegisters16)) {
            lexeme.type = "reg16";
        }
        else if (regex_match(token, generalRegisters8)) {
            lexeme.type = "reg8";
        }
        else if (regex_match(token, segmentRegisters)) {
            lexeme.type = "seg_reg";
        }
        else if (regex_match(token, binaryConst)) {
            lexeme.type = "imm_bin";
        }
        else if (regex_match(token, hexConst)) {
            lexeme.type = "imm_hex";
        }
        else if (regex_match(token, decimalConst)) {
            lexeme.type = "imm_dec";
        }
        else if (regex_match(token, textConst)) {
            lexeme.type = "txt_const";
        }
        else if (regex_match(token, symbol)) {
            lexeme.type = "symbol";
        }
        else if (regex_match(token, identifier)) {
            lexeme.type = "identifier";
        }
        else {
            lexeme.type = "unknown";
        }

        lexemes.push_back(lexeme);
    }


    std::vector<Lexeme> processedLexemes;
    for (size_t i = 0; i < lexemes.size(); ++i) 
    {
        if (i < lexemes.size() - 1 &&
            lexemes[i].value == "0B" &&
            regex_match(lexemes[i + 1].value, std::regex(R"([0-9A-F]+H)"))) {

            Lexeme combinedLexeme;
            combinedLexeme.value = lexemes[i].value + lexemes[i + 1].value;
            combinedLexeme.length = combinedLexeme.value.size();
            combinedLexeme.type = "imm_hex";
            combinedLexeme.index = lexemes[i].index;

            processedLexemes.push_back(combinedLexeme);
            i++; 
        }
        else 
        {
            processedLexemes.push_back(lexemes[i]);
        }
    }

    return processedLexemes;
}

void writeSentenceStructure(std::ofstream& outFile, const std::vector<Lexeme>& lexemes) {
    if (lexemes.empty()) return;

    std::map<std::string, std::pair<int, int>> structure; // key -> {index, count}

    int currentPos = 0;
    if (lexemes.size() > 1 && lexemes[0].type == "identifier" &&
        (lexemes[1].value == ":" || lexemes[1].type == "segment_start" || lexemes[1].type == "segment_ends")) 
    {structure["label"] = { lexemes[0].index, 1 };  

        if (lexemes[1].value == ":") 
        {
            currentPos = 2;
        }
        else 
        {
            structure["mnem"] = { lexemes[1].index, 1 };
            currentPos = 2;
        }
    }

    if (currentPos >= static_cast<int>(lexemes.size())) 
    {
        outFile << "SENTENCE STRUCT:\n";
        for (const auto& item : structure) {
            outFile << item.first << ": " << item.second.first << "," << item.second.second << " | ";
        }
        outFile << "\n\n";
        return;
    }

    if (lexemes[currentPos].type == "var-size" ||
        lexemes[currentPos].type == "instruction" ||
        lexemes[currentPos].type == "segment_start" ||
        lexemes[currentPos].type == "segment_ends" ||
        lexemes[currentPos].type == "program_end") 
    {
        structure["mnem"] = { lexemes[currentPos].index, 1 };
        currentPos++;
    }
    else if (structure.find("label") == structure.end() &&
        lexemes[currentPos].type == "identifier") {
        structure["label"] = { lexemes[currentPos].index, 1 };
        currentPos++;

        if (currentPos < static_cast<int>(lexemes.size()) &&
            (lexemes[currentPos].type == "var-size" ||
                lexemes[currentPos].type == "instruction" ||
                lexemes[currentPos].type == "segment_start" ||
                lexemes[currentPos].type == "segment_ends" ||
                lexemes[currentPos].type == "program_end")) {
            structure["mnem"] = { lexemes[currentPos].index, 1 };
            currentPos++;
        }
    }

    int opCount = 1;
    int opStart = currentPos;

    for (size_t i = currentPos; i < lexemes.size(); i++) 
    {
        if (lexemes[i].value == ",") 
        {
            if (i > opStart)
            {
                structure["op" + std::to_string(opCount)] = { lexemes[opStart].index, static_cast<int>(i - opStart) };
                opCount++;
            }
            opStart = i + 1;
        }
    }

    if (opStart < static_cast<int>(lexemes.size())) {
        if (opCount == 1) 
        {
            structure["op"] = { lexemes[opStart].index, static_cast<int>(lexemes.size() - opStart) };
        }
        else {
            structure["op" + std::to_string(opCount)] =
            { lexemes[opStart].index, static_cast<int>(lexemes.size() - opStart) };
        }
    }

    outFile << "SENTENCE STRUCT:\n";
    for (const auto& item : structure) {
        outFile << item.first << ": " << item.second.first << "," << item.second.second << " | ";
    }
    outFile << "\n\n";
}

void analyzeFile(const std::string& filename) 
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file\n";
        return;
    }

    std::ofstream output("lexemes_table.txt");
    if (!output.is_open()) 
    {
        std::cerr << "Failed to create output file\n";
        return;
    }

    std::string line;
    int lineNumber = 0;

    while (getline(file, line)) {
        lineNumber++;
        if (!line.empty()&&!isWhitespaceOnly(line)) 
        {
            output << "¹" << lineNumber << " | \"" << line << "\"\n";
            output << "Lexeme table:\n"<< "\t" << "Index" << "\t" << "Lexeme" << "\t\t" << "Length" << "\t\t" << "Type\n" ;

            std::vector<Lexeme> lexemes = analyzeLine(line);

            for (const auto& lex : lexemes) 
                output << "\t" << lex.index << "\t" << lex.value << "\t\t" << lex.length << "\t\t" << lex.type << "\n";

            writeSentenceStructure(output, lexemes);
            output << "\n";
        }
        else 
        {
            output << "¹" << lineNumber << " | \"<empty line>\"\n\n\n";
        }
    }

    file.close();
    output.close();
}