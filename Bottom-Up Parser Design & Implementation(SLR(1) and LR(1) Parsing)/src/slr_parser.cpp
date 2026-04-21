#include "slr_parser.h"
#include "stack.h"
#include <iostream>
#include <iomanip>
#include <fstream>

void SLRParser::buildParser(const std::string& grammarFile) {
    grammar.readFromFile(grammarFile);
    grammar.augmentGrammar();
    grammar.computeFirstSets();
    grammar.computeFollowSets();
    // Build production list
    for (const auto& prod : grammar.productions) {
        for (const auto& alt : prod.second) {
            productionList.push_back({prod.first, grammar.split(alt)});
        }
    }
    canonicalCollection.buildCanonicalCollection(grammar);
    buildActionTable();
    buildGotoTable();
}

void SLRParser::buildActionTable() {
    for (size_t i = 0; i < canonicalCollection.states.size(); ++i) {
        const ItemSet& I = canonicalCollection.states[i];
        for (const auto& item : I.items) {
            if (item.dotPosition < item.rhs.size()) {
                // Shift action
                std::string a = item.rhs[item.dotPosition];
                if (grammar.isTerminal(a)) {
                    auto it = canonicalCollection.transitions.find({i, a});
                    if (it != canonicalCollection.transitions.end()) {
                        int j = it->second;
                        actionTable[{i, a}] = Action(SHIFT, j);
                    }
                }
            } else {
                // Reduce action
                if (item.lhs == grammar.startSymbol && item.dotPosition == item.rhs.size()) {
                    // Accept
                    actionTable[{i, "$"}] = Action(ACCEPT, -1);
                } else {
                    // Reduce
                    for (const auto& a : grammar.follow[item.lhs]) {
                        if (a != "") {
                            actionTable[{i, a}] = Action(REDUCE, getProductionIndex(item));
                        }
                    }
                }
            }
        }
    }
}

void SLRParser::buildGotoTable() {
    for (const auto& trans : canonicalCollection.transitions) {
        int i = trans.first.first;
        std::string X = trans.first.second;
        int j = trans.second;
        if (grammar.isNonTerminal(X)) {
            gotoTable[{i, X}] = j;
        }
    }
}

int SLRParser::getProductionIndex(const LR0Item& item) {
    std::string rhsStr = joinRhs(item.rhs);
    for (size_t i = 0; i < productionList.size(); ++i) {
        if (productionList[i].first == item.lhs && joinRhs(productionList[i].second) == rhsStr) {
            return i;
        }
    }
    return -1;
}

std::string SLRParser::joinRhs(const std::vector<std::string>& rhs) {
    std::string result;
    for (size_t i = 0; i < rhs.size(); ++i) {
        if (i > 0) result += " ";
        result += rhs[i];
    }
    return result;
}

bool SLRParser::parse(const std::vector<std::string>& input) {
    Stack stack;
    stack.push("", 0); // initial state
    size_t inputIndex = 0;
    std::vector<std::string> inputWithEnd = input;
    inputWithEnd.push_back("$");

    while (true) {
        int s = stack.topState();
        std::string a = inputWithEnd[inputIndex];
        auto actionIt = actionTable.find({s, a});
        if (actionIt == actionTable.end()) {
            std::cout << "Error: No action for state " << s << " and symbol " << a << std::endl;
            return false;
        }
        Action action = actionIt->second;

        if (action.type == SHIFT) {
            stack.push(a, action.value);
            inputIndex++;
        } else if (action.type == REDUCE) {
            // Find the production
            int prodIndex = action.value;
            std::string A;
            std::vector<std::string> beta;
            getProductionByIndex(prodIndex, A, beta);

            // Pop |beta| elements
            for (size_t i = 0; i < beta.size(); ++i) {
                stack.pop();
            }

            int t = stack.topState();
            auto gotoIt = gotoTable.find({t, A});
            if (gotoIt == gotoTable.end()) {
                std::cout << "Error: No goto for state " << t << " and non-terminal " << A << std::endl;
                return false;
            }
            int newState = gotoIt->second;
            stack.push(A, newState);
        } else if (action.type == ACCEPT) {
            std::cout << "Parsing successful!" << std::endl;
            return true;
        } else {
            std::cout << "Error: Invalid action" << std::endl;
            return false;
        }
    }
}

void SLRParser::getProductionByIndex(int index, std::string& A, std::vector<std::string>& beta) {
    if (index >= 0 && index < productionList.size()) {
        A = productionList[index].first;
        beta = productionList[index].second;
    }
}

void SLRParser::printParsingTable() {
    std::cout << "SLR(1) Parsing Table:" << std::endl;
    // Print header
    std::cout << std::setw(5) << "State";
    for (const auto& term : grammar.terminals) {
        std::cout << std::setw(10) << term;
    }
    std::cout << std::setw(5) << "$";
    for (const auto& nt : grammar.nonTerminals) {
        std::cout << std::setw(10) << nt;
    }
    std::cout << std::endl;

    for (size_t i = 0; i < canonicalCollection.states.size(); ++i) {
        std::cout << std::setw(5) << i;
        for (const auto& term : grammar.terminals) {
            auto it = actionTable.find({i, term});
            if (it != actionTable.end()) {
                Action a = it->second;
                if (a.type == SHIFT) std::cout << std::setw(10) << "s" + std::to_string(a.value);
                else if (a.type == REDUCE) std::cout << std::setw(10) << "r" + std::to_string(a.value);
                else std::cout << std::setw(10) << "";
            } else {
                std::cout << std::setw(10) << "";
            }
        }
        // $
        auto it = actionTable.find({i, "$"});
        if (it != actionTable.end()) {
            Action a = it->second;
            if (a.type == ACCEPT) std::cout << std::setw(5) << "acc";
            else std::cout << std::setw(5) << "";
        } else {
            std::cout << std::setw(5) << "";
        }
        // GOTO
        for (const auto& nt : grammar.nonTerminals) {
            auto git = gotoTable.find({i, nt});
            if (git != gotoTable.end()) {
                std::cout << std::setw(10) << git->second;
            } else {
                std::cout << std::setw(10) << "";
            }
        }
        std::cout << std::endl;
    }
}

void SLRParser::printTrace(const std::vector<std::string>& input) {
    // Similar to parse but print steps
    Stack stack;
    stack.push("", 0);
    size_t inputIndex = 0;
    std::vector<std::string> inputWithEnd = input;
    inputWithEnd.push_back("$");
    int step = 1;

    std::cout << "Parsing Trace:" << std::endl;
    std::cout << std::setw(5) << "Step" << std::setw(15) << "Stack" << std::setw(20) << "Input" << std::setw(15) << "Action" << std::endl;

    while (true) {
        // Print current state
        std::cout << std::setw(5) << step;
        std::cout << std::setw(15) << stack.toString();
        std::string remainingInput;
        for (size_t j = inputIndex; j < inputWithEnd.size(); ++j) {
            remainingInput += inputWithEnd[j] + " ";
        }
        std::cout << std::setw(20) << remainingInput;

        int s = stack.topState();
        std::string a = inputWithEnd[inputIndex];
        auto actionIt = actionTable.find({s, a});
        if (actionIt == actionTable.end()) {
            std::cout << std::setw(15) << "Error" << std::endl;
            return;
        }
        Action action = actionIt->second;

        if (action.type == SHIFT) {
            std::cout << std::setw(15) << "Shift " + std::to_string(action.value) << std::endl;
            stack.push(a, action.value);
            inputIndex++;
        } else if (action.type == REDUCE) {
            std::string A;
            std::vector<std::string> beta;
            getProductionByIndex(action.value, A, beta);
            std::string prodStr = A + " -> " + joinRhs(beta);
            std::cout << std::setw(15) << "Reduce " + prodStr << std::endl;

            for (size_t i = 0; i < beta.size(); ++i) {
                stack.pop();
            }
            int t = stack.topState();
            auto gotoIt = gotoTable.find({t, A});
            int newState = gotoIt->second;
            stack.push(A, newState);
        } else if (action.type == ACCEPT) {
            std::cout << std::setw(15) << "Accept" << std::endl;
            return;
        } else {
            std::cout << std::setw(15) << "Error" << std::endl;
            return;
        }
        step++;
    }
}