#include "grammar.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
using namespace std;

Grammar::Grammar() {}

void Grammar::readFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        parseProduction(line);
    }

    // Set start symbol if not set
    if (startSymbol.empty() && !productions.empty()) {
        startSymbol = productions.begin()->first;
    }
}

void Grammar::parseProduction(const string& line) {
    istringstream iss(line);
    string lhs, arrow, rhs;
    iss >> lhs >> arrow;
    if (arrow != "->") {
        cerr << "Invalid production format: " << line << endl;
        return;
    }

    getline(iss, rhs);
    rhs.erase(0, rhs.find_first_not_of(" \t")); // trim leading spaces

    vector<string> alternatives;
    istringstream rhsStream(rhs);
    string alternative;
    while (getline(rhsStream, alternative, '|')) {
        alternative.erase(alternative.find_last_not_of(" \t") + 1);
        alternative.erase(0, alternative.find_first_not_of(" \t"));
        if (!alternative.empty()) {
            alternatives.push_back(alternative);
        }
    }

    productions[lhs] = alternatives;
    nonTerminals.insert(lhs);
}

void Grammar::augmentGrammar() {
    string newStart = startSymbol + "'";
    vector<string> newProd = {startSymbol};
    productions[newStart] = newProd;
    startSymbol = newStart;
    nonTerminals.insert(newStart);
}

void Grammar::computeFirstSets() {
    // Initialize FIRST sets
    for (const auto& nt : nonTerminals) {
        first[nt] = set<string>();
    }
    for (const auto& prod : productions) {
        for (const auto& alt : prod.second) {
            istringstream iss(alt);
            string symbol;
            while (iss >> symbol) {
                if (isTerminal(symbol)) {
                    terminals.insert(symbol);
                }
            }
        }
    }

    bool changed = true;
    while (changed) {
        changed = false;
        for (const auto& prod : productions) {
            string A = prod.first;
            for (const auto& alpha : prod.second) {
                set<string> firstAlpha = computeFirst(alpha);
                size_t oldSize = first[A].size();
                first[A].insert(firstAlpha.begin(), firstAlpha.end());
                if (first[A].size() > oldSize) changed = true;
            }
        }
    }
}

set<string> Grammar::computeFirst(const string& alpha) {
    set<string> result;
    istringstream iss(alpha);
    string X;
    bool allNullable = true;

    while (iss >> X) {
        if (isTerminal(X) || X == "epsilon" || X == "@") {
            result.insert(X == "epsilon" || X == "@" ? "" : X);
            allNullable = false;
            break;
        } else {
            // X is non-terminal
            result.insert(first[X].begin(), first[X].end());
            if (first[X].find("") == first[X].end()) {
                allNullable = false;
                break;
            }
            result.erase(""); // remove epsilon if not all nullable
        }
    }

    if (allNullable) {
        result.insert("");
    }

    return result;
}

void Grammar::computeFollowSets() {
    // Initialize FOLLOW sets
    for (const auto& nt : nonTerminals) {
        follow[nt] = set<string>();
    }
    follow[startSymbol].insert("$");

    bool changed = true;
    while (changed) {
        changed = false;
        for (const auto& prod : productions) {
            string A = prod.first;
            for (const auto& alpha : prod.second) {
                vector<string> symbols = split(alpha);
                for (size_t i = 0; i < symbols.size(); ++i) {
                    if (isNonTerminal(symbols[i])) {
                        string B = symbols[i];
                        set<string> firstBeta;
                        if (i + 1 < symbols.size()) {
                            string beta = join(symbols.begin() + i + 1, symbols.end());
                            firstBeta = computeFirst(beta);
                        }

                        size_t oldSize = follow[B].size();
                        for (const auto& sym : firstBeta) {
                            if (sym != "") {
                                follow[B].insert(sym);
                            }
                        }

                        if (firstBeta.find("") != firstBeta.end() || i + 1 == symbols.size()) {
                            follow[B].insert(follow[A].begin(), follow[A].end());
                        }

                        if (follow[B].size() > oldSize) changed = true;
                    }
                }
            }
        }
    }
}

vector<string> Grammar::split(const string& s) {
    vector<string> result;
    istringstream iss(s);
    string token;
    while (iss >> token) {
        result.push_back(token);
    }
    return result;
}

string Grammar::join(const vector<string>::iterator start, const vector<string>::iterator end) {
    string result;
    for (auto it = start; it != end; ++it) {
        if (it != start) result += " ";
        result += *it;
    }
    return result;
}

bool Grammar::isTerminal(const string& symbol) {
    return nonTerminals.find(symbol) == nonTerminals.end();
}

bool Grammar::isNonTerminal(const string& symbol) {
    return nonTerminals.find(symbol) != nonTerminals.end();
}

void Grammar::printAugmentedGrammar() {
    cout << "Augmented Grammar:" << endl;
    for (const auto& prod : productions) {
        cout << prod.first << " -> ";
        for (size_t i = 0; i < prod.second.size(); ++i) {
            if (i > 0) cout << " | ";
            cout << prod.second[i];
        }
        cout << endl;
    }
}
