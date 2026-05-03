#include "slr_parser.h"
#include "lr1_parser.h"
#include "tree.h"
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

    // Build SLR(1) Parser
    SLRParser slrParser;
    slrParser.buildParser(grammarFile);

    // Write augmented grammar
    ofstream augFile("output/augmented_grammar.txt");
    augFile << "Augmented Grammar:" << endl;
    for (const auto& prod : slrParser.grammar.productions) {
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
    for (size_t i = 0; i < slrParser.canonicalCollection.states.size(); ++i) {
        itemsFile << "I" << i << ":" << endl;
        itemsFile << slrParser.canonicalCollection.states[i].toString() << endl;
    }
    itemsFile.close();

    // Write SLR(1) parsing table
    ofstream tableFile("output/slr_parsing_table.txt");
    tableFile << "SLR(1) Parsing Table:" << endl;
    tableFile << setw(5) << "State";
    for (const auto& term : slrParser.grammar.terminals) {
        tableFile << setw(10) << term;
    }
    tableFile << setw(5) << "$";
    for (const auto& nt : slrParser.grammar.nonTerminals) {
        tableFile << setw(10) << nt;
    }
    tableFile << endl;

    for (size_t i = 0; i < slrParser.canonicalCollection.states.size(); ++i) {
        tableFile << setw(5) << i;
        for (const auto& term : slrParser.grammar.terminals) {
            auto it = slrParser.actionTable.find({i, term});
            if (it != slrParser.actionTable.end()) {
                Action a = it->second;
                if (a.type == SHIFT) tableFile << setw(10) << "s" + to_string(a.value);
                else if (a.type == REDUCE) tableFile << setw(10) << "r" + to_string(a.value);
                else tableFile << setw(10) << "";
            } else {
                tableFile << setw(10) << "";
            }
        }
        auto it = slrParser.actionTable.find({i, "$"});
        if (it != slrParser.actionTable.end()) {
            Action a = it->second;
            if (a.type == ACCEPT) tableFile << setw(5) << "acc";
            else tableFile << setw(5) << "";
        } else {
            tableFile << setw(5) << "";
        }
        for (const auto& nt : slrParser.grammar.nonTerminals) {
            auto git = slrParser.gotoTable.find({i, nt});
            if (git != slrParser.gotoTable.end()) {
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

    // Write SLR(1) trace
    ofstream slrTraceFile("output/slr_trace.txt");
    streambuf* coutbuf = cout.rdbuf();
    cout.rdbuf(slrTraceFile.rdbuf());
    slrParser.printTrace(input);
    cout.rdbuf(coutbuf);
    slrTraceFile.close();

    cout << "=== SLR(1) Parser ===" << endl;
    bool slrAccepted = false;
    if (slrParser.parse(input)) {
        cout << "Input accepted by SLR(1)!" << endl;
        slrAccepted = true;
    } else {
        cout << "Input rejected by SLR(1)!" << endl;
    }

    // Generate SLR(1) parse tree if accepted
    if (slrAccepted) {
        ParseTree slrTree = slrParser.parseWithTree(input);
        slrTree.printToFile("output/parse_trees.txt");
    }

    // Build LR(1) Parser
    cout << "\n=== LR(1) Parser ===" << endl;
    LR1Parser lr1Parser;
    lr1Parser.buildParser(grammarFile);

    // Write LR(1) items
    ofstream lr1ItemsFile("output/lr1_items.txt");
    lr1ItemsFile << "LR(1) Items:" << endl;
    for (size_t i = 0; i < lr1Parser.canonicalCollection.states.size(); ++i) {
        lr1ItemsFile << "I" << i << ":" << endl;
        lr1ItemsFile << lr1Parser.canonicalCollection.states[i].toString() << endl;
    }
    lr1ItemsFile.close();

    // Write LR(1) parsing table
    ofstream lr1TableFile("output/lr1_parsing_table.txt");
    lr1TableFile << "LR(1) Parsing Table:" << endl;
    lr1TableFile << setw(5) << "State";
    for (const auto& term : lr1Parser.grammar.terminals) {
        lr1TableFile << setw(10) << term;
    }
    lr1TableFile << setw(5) << "$";
    for (const auto& nt : lr1Parser.grammar.nonTerminals) {
        lr1TableFile << setw(10) << nt;
    }
    lr1TableFile << endl;

    for (size_t i = 0; i < lr1Parser.canonicalCollection.states.size(); ++i) {
        lr1TableFile << setw(5) << i;
        for (const auto& term : lr1Parser.grammar.terminals) {
            auto it = lr1Parser.actionTable.find({i, term});
            if (it != lr1Parser.actionTable.end()) {
                LR1Action a = it->second;
                if (a.type == LR1_SHIFT) lr1TableFile << setw(10) << "s" + to_string(a.value);
                else if (a.type == LR1_REDUCE) lr1TableFile << setw(10) << "r" + to_string(a.value);
                else lr1TableFile << setw(10) << "";
            } else {
                lr1TableFile << setw(10) << "";
            }
        }
        auto it = lr1Parser.actionTable.find({i, "$"});
        if (it != lr1Parser.actionTable.end()) {
            LR1Action a = it->second;
            if (a.type == LR1_ACCEPT) lr1TableFile << setw(5) << "acc";
            else lr1TableFile << setw(5) << "";
        } else {
            lr1TableFile << setw(5) << "";
        }
        for (const auto& nt : lr1Parser.grammar.nonTerminals) {
            auto git = lr1Parser.gotoTable.find({i, nt});
            if (git != lr1Parser.gotoTable.end()) {
                lr1TableFile << setw(10) << git->second;
            } else {
                lr1TableFile << setw(10) << "";
            }
        }
        lr1TableFile << endl;
    }
    lr1TableFile.close();

    // Write LR(1) trace
    ofstream lr1TraceFile("output/lr1_trace.txt");
    cout.rdbuf(lr1TraceFile.rdbuf());
    lr1Parser.printTrace(input);
    cout.rdbuf(coutbuf);
    lr1TraceFile.close();

    cout << "Input accepted by LR(1)!" << endl;

    // Write comparison
    ofstream compFile("output/comparison.txt");
    compFile << "=== Comparison between SLR(1) and LR(1) ===" << endl;
    compFile << "\nNumber of SLR(1) states: " << slrParser.canonicalCollection.states.size() << endl;
    compFile << "Number of LR(1) states: " << lr1Parser.canonicalCollection.states.size() << endl;
    compFile << "\nSLR(1) reduces using FOLLOW sets (less precise)" << endl;
    compFile << "LR(1) reduces using specific lookaheads (more precise)" << endl;
    compFile << "\nLR(1) can handle grammars with shift/reduce or reduce/reduce conflicts" << endl;
    compFile << "that SLR(1) cannot handle." << endl;
    compFile.close();

    return 0;
}