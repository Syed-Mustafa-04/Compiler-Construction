#include "slr_parser.h"
#include "lr1_parser.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>

std::vector<std::string> readInput(const std::string& filename) {
    std::vector<std::string> tokens;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening input file: " << filename << std::endl;
        return tokens;
    }
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        while (iss >> token) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

void writeSLROutputs(SLRParser& parser, const std::vector<std::string>& input) {
    // Write augmented grammar
    std::ofstream augFile("output/augmented_grammar.txt");
    augFile << "Augmented Grammar:" << std::endl;
    for (const auto& prod : parser.grammar.productions) {
        augFile << prod.first << " -> ";
        for (size_t i = 0; i < prod.second.size(); ++i) {
            if (i > 0) augFile << " | ";
            augFile << prod.second[i];
        }
        augFile << std::endl;
    }
    augFile.close();

    // Write LR(0) items
    std::ofstream itemsFile("output/slr_items.txt");
    itemsFile << "SLR(1) - LR(0) Items:" << std::endl;
    for (size_t i = 0; i < parser.canonicalCollection.states.size(); ++i) {
        itemsFile << "I" << i << ":" << std::endl;
        itemsFile << parser.canonicalCollection.states[i].toString() << std::endl;
    }
    itemsFile.close();

    // Write parsing table
    std::ofstream tableFile("output/slr_parsing_table.txt");
    tableFile << "SLR(1) Parsing Table:" << std::endl;
    tableFile << std::setw(5) << "State";
    for (const auto& term : parser.grammar.terminals) {
        tableFile << std::setw(10) << term;
    }
    tableFile << std::setw(5) << "$";
    for (const auto& nt : parser.grammar.nonTerminals) {
        tableFile << std::setw(10) << nt;
    }
    tableFile << std::endl;

    for (size_t i = 0; i < parser.canonicalCollection.states.size(); ++i) {
        tableFile << std::setw(5) << i;
        for (const auto& term : parser.grammar.terminals) {
            auto it = parser.actionTable.find({i, term});
            if (it != parser.actionTable.end()) {
                Action a = it->second;
                if (a.type == SHIFT) tableFile << std::setw(10) << "s" + std::to_string(a.value);
                else if (a.type == REDUCE) tableFile << std::setw(10) << "r" + std::to_string(a.value);
                else tableFile << std::setw(10) << "";
            } else {
                tableFile << std::setw(10) << "";
            }
        }
        // $
        auto it = parser.actionTable.find({i, "$"});
        if (it != parser.actionTable.end()) {
            Action a = it->second;
            if (a.type == ACCEPT) tableFile << std::setw(5) << "acc";
            else tableFile << std::setw(5) << "";
        } else {
            tableFile << std::setw(5) << "";
        }
        // GOTO
        for (const auto& nt : parser.grammar.nonTerminals) {
            auto git = parser.gotoTable.find({i, nt});
            if (git != parser.gotoTable.end()) {
                tableFile << std::setw(10) << git->second;
            } else {
                tableFile << std::setw(10) << "";
            }
        }
        tableFile << std::endl;
    }
    tableFile.close();

    // Write trace
    std::ofstream traceFile("output/slr_trace.txt");
    std::streambuf* coutbuf = std::cout.rdbuf();
    std::cout.rdbuf(traceFile.rdbuf());
    parser.printTrace(input);
    std::cout.rdbuf(coutbuf);
    traceFile.close();
}

void writeLR1Outputs(LR1Parser& parser, const std::vector<std::string>& input) {
    // Write LR(1) items
    std::ofstream itemsFile("output/lr1_items.txt");
    itemsFile << "LR(1) Items:" << std::endl;
    for (size_t i = 0; i < parser.canonicalCollection.states.size(); ++i) {
        itemsFile << "I" << i << ":" << std::endl;
        itemsFile << parser.canonicalCollection.states[i].toString() << std::endl;
    }
    itemsFile.close();

    // Write parsing table
    std::ofstream tableFile("output/lr1_parsing_table.txt");
    tableFile << "LR(1) Parsing Table:" << std::endl;
    tableFile << std::setw(5) << "State";
    for (const auto& term : parser.grammar.terminals) {
        tableFile << std::setw(10) << term;
    }
    tableFile << std::setw(5) << "$";
    for (const auto& nt : parser.grammar.nonTerminals) {
        tableFile << std::setw(10) << nt;
    }
    tableFile << std::endl;

    for (size_t i = 0; i < parser.canonicalCollection.states.size(); ++i) {
        tableFile << std::setw(5) << i;
        for (const auto& term : parser.grammar.terminals) {
            auto it = parser.actionTable.find({i, term});
            if (it != parser.actionTable.end()) {
                Action a = it->second;
                if (a.type == SHIFT) tableFile << std::setw(10) << "s" + std::to_string(a.value);
                else if (a.type == REDUCE) tableFile << std::setw(10) << "r" + std::to_string(a.value);
                else tableFile << std::setw(10) << "";
            } else {
                tableFile << std::setw(10) << "";
            }
        }
        // $
        auto it = parser.actionTable.find({i, "$"});
        if (it != parser.actionTable.end()) {
            Action a = it->second;
            if (a.type == ACCEPT) tableFile << std::setw(5) << "acc";
            else tableFile << std::setw(5) << "";
        } else {
            tableFile << std::setw(5) << "";
        }
        // GOTO
        for (const auto& nt : parser.grammar.nonTerminals) {
            auto git = parser.gotoTable.find({i, nt});
            if (git != parser.gotoTable.end()) {
                tableFile << std::setw(10) << git->second;
            } else {
                tableFile << std::setw(10) << "";
            }
        }
        tableFile << std::endl;
    }
    tableFile.close();

    // Write trace
    std::ofstream traceFile("output/lr1_trace.txt");
    std::streambuf* coutbuf = std::cout.rdbuf();
    std::cout.rdbuf(traceFile.rdbuf());
    parser.printTrace(input);
    std::cout.rdbuf(coutbuf);
    traceFile.close();
}

void writeComparison(SLRParser& slrParser, LR1Parser& lr1Parser) {
    std::ofstream compFile("output/comparison.txt");
    compFile << "SLR(1) vs LR(1) Parser Comparison" << std::endl;
    compFile << "==================================" << std::endl;
    compFile << "\nGrammar Statistics:" << std::endl;
    compFile << "Number of Non-terminals: " << slrParser.grammar.nonTerminals.size() << std::endl;
    compFile << "Number of Terminals: " << slrParser.grammar.terminals.size() << std::endl;
    
    compFile << "\nCanonical Collection Sizes:" << std::endl;
    compFile << "SLR(1) - LR(0) States: " << slrParser.canonicalCollection.states.size() << std::endl;
    compFile << "LR(1) States: " << lr1Parser.canonicalCollection.states.size() << std::endl;
    
    compFile << "\nParsing Table Sizes:" << std::endl;
    compFile << "SLR(1) ACTION entries: " << slrParser.actionTable.size() << std::endl;
    compFile << "SLR(1) GOTO entries: " << slrParser.gotoTable.size() << std::endl;
    compFile << "LR(1) ACTION entries: " << lr1Parser.actionTable.size() << std::endl;
    compFile << "LR(1) GOTO entries: " << lr1Parser.gotoTable.size() << std::endl;
    
    compFile << "\nKey Differences:" << std::endl;
    compFile << "- SLR(1) uses FOLLOW sets for reductions (may cause conflicts)" << std::endl;
    compFile << "- LR(1) uses lookahead terminals from items (more precise)" << std::endl;
    compFile << "- LR(1) typically generates more states than SLR(1)" << std::endl;
    compFile << "- LR(1) resolves conflicts that SLR(1) cannot handle" << std::endl;
    
    compFile.close();
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <grammar_file> <input_file> [slr|lr1|both]" << std::endl;
        std::cout << "Default: both" << std::endl;
        return 1;
    }

    std::string grammarFile = argv[1];
    std::string inputFile = argv[2];
    std::string parserType = (argc >= 4) ? argv[3] : "both";

    std::vector<std::string> input = readInput(inputFile);

    if (parserType == "slr" || parserType == "both") {
        std::cout << "\n=== Building SLR(1) Parser ===" << std::endl;
        SLRParser slrParser;
        slrParser.buildParser(grammarFile);
        std::cout << "SLR(1) Parser built successfully!" << std::endl;
        std::cout << "Number of states: " << slrParser.canonicalCollection.states.size() << std::endl;
        
        writeSLROutputs(slrParser, input);
        
        std::cout << "\nParsing with SLR(1)..." << std::endl;
        if (slrParser.parse(input)) {
            std::cout << "SLR(1): Input accepted!" << std::endl;
        } else {
            std::cout << "SLR(1): Input rejected!" << std::endl;
        }
    }

    if (parserType == "lr1" || parserType == "both") {
        std::cout << "\n=== Building LR(1) Parser ===" << std::endl;
        LR1Parser lr1Parser;
        lr1Parser.buildParser(grammarFile);
        std::cout << "LR(1) Parser built successfully!" << std::endl;
        std::cout << "Number of states: " << lr1Parser.canonicalCollection.states.size() << std::endl;
        
        writeLR1Outputs(lr1Parser, input);
        
        std::cout << "\nParsing with LR(1)..." << std::endl;
        if (lr1Parser.parse(input)) {
            std::cout << "LR(1): Input accepted!" << std::endl;
        } else {
            std::cout << "LR(1): Input rejected!" << std::endl;
        }
    }

    if (parserType == "both") {
        std::cout << "\n=== Writing Comparison ===" << std::endl;
        SLRParser slrParser;
        slrParser.buildParser(grammarFile);
        LR1Parser lr1Parser;
        lr1Parser.buildParser(grammarFile);
        writeComparison(slrParser, lr1Parser);
        std::cout << "Comparison written to output/comparison.txt" << std::endl;
    }

    std::cout << "\nOutput files written to output/ directory" << std::endl;
    return 0;
}
