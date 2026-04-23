#include "slr_parser.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>
using namespace std;

vector<string> readInput(const string& filename) {
    vector<string> tokens;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening input file: " << filename << endl;
        return tokens;
    }
    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string token;
        while (iss >> token) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Usage: " << argv[0] << " <grammar_file> <input_file>" << endl;
        return 1;
    }

    string grammarFile = argv[1];
    string inputFile = argv[2];

    SLRParser parser;
    parser.buildParser(grammarFile);

    // Write augmented grammar
    ofstream augFile("output/augmented_grammar.txt");
    augFile << "Augmented Grammar:" << endl;
    for (const auto& prod : parser.grammar.productions) {
        augFile << prod.first << " -> ";
        for (size_t i = 0; i < prod.second.size(); ++i) {
            if (i > 0) augFile << " | ";
            augFile << prod.second[i];
        }
        augFile << endl;
    }
    augFile.close();

    // Write LR(0) items
    ofstream itemsFile("output/slr_items.txt");
    itemsFile << "LR(0) Items:" << endl;
    for (size_t i = 0; i < parser.canonicalCollection.states.size(); ++i) {
        itemsFile << "I" << i << ":" << endl;
        itemsFile << parser.canonicalCollection.states[i].toString() << endl;
    }
    itemsFile.close();

    // Write parsing table
    ofstream tableFile("output/slr_parsing_table.txt");
    tableFile << "SLR(1) Parsing Table:" << endl;
    // Similar to printParsingTable but to file
    tableFile << setw(5) << "State";
    for (const auto& term : parser.grammar.terminals) {
        tableFile << setw(10) << term;
    }
    tableFile << setw(5) << "$";
    for (const auto& nt : parser.grammar.nonTerminals) {
        tableFile << setw(10) << nt;
    }
    tableFile << endl;

    for (size_t i = 0; i < parser.canonicalCollection.states.size(); ++i) {
        tableFile << setw(5) << i;
        for (const auto& term : parser.grammar.terminals) {
            auto it = parser.actionTable.find({i, term});
            if (it != parser.actionTable.end()) {
                Action a = it->second;
                if (a.type == SHIFT) tableFile << setw(10) << "s" + to_string(a.value);
                else if (a.type == REDUCE) tableFile << setw(10) << "r" + to_string(a.value);
                else tableFile << setw(10) << "";
            } else {
                tableFile << setw(10) << "";
            }
        }
        // $
        auto it = parser.actionTable.find({i, "$"});
        if (it != parser.actionTable.end()) {
            Action a = it->second;
            if (a.type == ACCEPT) tableFile << setw(5) << "acc";
            else tableFile << setw(5) << "";
        } else {
            tableFile << setw(5) << "";
        }
        // GOTO
        for (const auto& nt : parser.grammar.nonTerminals) {
            auto git = parser.gotoTable.find({i, nt});
            if (git != parser.gotoTable.end()) {
                tableFile << setw(10) << git->second;
            } else {
                tableFile << setw(10) << "";
            }
        }
        tableFile << endl;
    }
    tableFile.close();

    // Read input
    vector<string> input = readInput(inputFile);

    // Write trace
    ofstream traceFile("output/slr_trace.txt");
    // Redirect cout to file for trace
    streambuf* coutbuf = cout.rdbuf();
    cout.rdbuf(traceFile.rdbuf());
    parser.printTrace(input);
    cout.rdbuf(coutbuf);
    traceFile.close();

    // Check if accepted
    if (parser.parse(input)) {
        cout << "Input accepted!" << endl;
    } else {
        cout << "Input rejected!" << endl;
    }

    return 0;
}
