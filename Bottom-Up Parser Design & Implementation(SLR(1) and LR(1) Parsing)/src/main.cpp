#include "slr_parser.h"
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

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <grammar_file> <input_file>" << std::endl;
        return 1;
    }

    std::string grammarFile = argv[1];
    std::string inputFile = argv[2];

    SLRParser parser;
    parser.buildParser(grammarFile);

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
    itemsFile << "LR(0) Items:" << std::endl;
    for (size_t i = 0; i < parser.canonicalCollection.states.size(); ++i) {
        itemsFile << "I" << i << ":" << std::endl;
        itemsFile << parser.canonicalCollection.states[i].toString() << std::endl;
    }
    itemsFile.close();

    // Write parsing table
    std::ofstream tableFile("output/slr_parsing_table.txt");
    tableFile << "SLR(1) Parsing Table:" << std::endl;
    // Similar to printParsingTable but to file
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

    // Read input
    std::vector<std::string> input = readInput(inputFile);

    // Write trace
    std::ofstream traceFile("output/slr_trace.txt");
    // Redirect cout to file for trace
    std::streambuf* coutbuf = std::cout.rdbuf();
    std::cout.rdbuf(traceFile.rdbuf());
    parser.printTrace(input);
    std::cout.rdbuf(coutbuf);
    traceFile.close();

    // Check if accepted
    if (parser.parse(input)) {
        std::cout << "Input accepted!" << std::endl;
    } else {
        std::cout << "Input rejected!" << std::endl;
    }

    return 0;
}