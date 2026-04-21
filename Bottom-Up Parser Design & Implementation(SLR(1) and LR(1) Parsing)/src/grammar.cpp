#include "grammar.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

Grammar::Grammar() {}

void Grammar::readFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        parseProduction(line);
    }

    // Set start symbol if not set
    if (startSymbol.empty() && !productions.empty()) {
        startSymbol = productions.begin()->first;
    }
}

void Grammar::parseProduction(const std::string& line) {
    std::istringstream iss(line);
    std::string lhs, arrow, rhs;
    iss >> lhs >> arrow;
    if (arrow != "->") {
        std::cerr << "Invalid production format: " << line << std::endl;
        return;
    }

    std::getline(iss, rhs);
    rhs.erase(0, rhs.find_first_not_of(" \t")); // trim leading spaces

    std::vector<std::string> alternatives;
    std::istringstream rhsStream(rhs);
    std::string alternative;
    while (std::getline(rhsStream, alternative, '|')) {
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
    std::string newStart = startSymbol + "'";
    std::vector<std::string> newProd = {startSymbol};
    productions[newStart] = newProd;
    startSymbol = newStart;
    nonTerminals.insert(newStart);
}

void Grammar::computeFirstSets() {
    // Initialize FIRST sets
    for (const auto& nt : nonTerminals) {
        first[nt] = std::set<std::string>();
    }
    for (const auto& prod : productions) {
        for (const auto& alt : prod.second) {
            std::istringstream iss(alt);
            std::string symbol;
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
            std::string A = prod.first;
            for (const auto& alpha : prod.second) {
                std::set<std::string> firstAlpha = computeFirst(alpha);
                size_t oldSize = first[A].size();
                first[A].insert(firstAlpha.begin(), firstAlpha.end());
                if (first[A].size() > oldSize) changed = true;
            }
        }
    }
}

std::set<std::string> Grammar::computeFirst(const std::string& alpha) {
    std::set<std::string> result;
    std::istringstream iss(alpha);
    std::string X;
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
        follow[nt] = std::set<std::string>();
    }
    follow[startSymbol].insert("$");

    bool changed = true;
    while (changed) {
        changed = false;
        for (const auto& prod : productions) {
            std::string A = prod.first;
            for (const auto& alpha : prod.second) {
                std::vector<std::string> symbols = split(alpha);
                for (size_t i = 0; i < symbols.size(); ++i) {
                    if (isNonTerminal(symbols[i])) {
                        std::string B = symbols[i];
                        std::set<std::string> firstBeta;
                        if (i + 1 < symbols.size()) {
                            std::string beta = join(symbols.begin() + i + 1, symbols.end());
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

std::vector<std::string> Grammar::split(const std::string& s) {
    std::vector<std::string> result;
    std::istringstream iss(s);
    std::string token;
    while (iss >> token) {
        result.push_back(token);
    }
    return result;
}

std::string Grammar::join(const std::vector<std::string>::iterator start, const std::vector<std::string>::iterator end) {
    std::string result;
    for (auto it = start; it != end; ++it) {
        if (it != start) result += " ";
        result += *it;
    }
    return result;
}

bool Grammar::isTerminal(const std::string& symbol) {
    return nonTerminals.find(symbol) == nonTerminals.end();
}

bool Grammar::isNonTerminal(const std::string& symbol) {
    return nonTerminals.find(symbol) != nonTerminals.end();
}

void Grammar::printAugmentedGrammar() {
    std::cout << "Augmented Grammar:" << std::endl;
    for (const auto& prod : productions) {
        std::cout << prod.first << " -> ";
        for (size_t i = 0; i < prod.second.size(); ++i) {
            if (i > 0) std::cout << " | ";
            std::cout << prod.second[i];
        }
        std::cout << std::endl;
    }
}