#include "lr1_parser.h"
#include "stack.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
using namespace std;

// LR1Item toString method
string LR1Item::toString() const {
    string result = "[" + lhs + " -> ";
    for (size_t i = 0; i < rhs.size(); ++i) {
        if (i == dotPosition) result += "• ";
        result += rhs[i] + " ";
    }
    if (dotPosition == rhs.size()) result += "•";
    result += ", " + lookahead + "]";
    return result;
}

// LR1ItemSet toString method
string LR1ItemSet::toString() const {
    string result;
    for (const auto& item : items) {
        result += item.toString() + "\n";
    }
    return result;
}

// Compute FIRST set for a string of symbols
set<string> computeFirstForString(const string& str, Grammar& grammar, bool& allNullable) {
    set<string> result;
    allNullable = true;
    
    vector<string> symbols = grammar.split(str);
    for (const auto& X : symbols) {
        if (grammar.isTerminal(X)) {
            result.insert(X);
            allNullable = false;
            break;
        } else {
            // X is non-terminal
            for (const auto& sym : grammar.first[X]) {
                if (sym != "") {
                    result.insert(sym);
                }
            }
            if (grammar.first[X].find("") == grammar.first[X].end()) {
                allNullable = false;
                break;
            }
        }
    }
    
    return result;
}

// LR1ItemSet closure operation
LR1ItemSet CanonicalLR1Collection::closure(const LR1ItemSet& I, Grammar& grammar) {
    LR1ItemSet closureSet = I;
    bool changed = true;
    
    while (changed) {
        changed = false;
        set<LR1Item> newItems;
        
        for (const auto& item : closureSet.items) {
            // If there's a non-terminal after the dot
            if (item.dotPosition < item.rhs.size()) {
                string B = item.rhs[item.dotPosition];
                if (grammar.isNonTerminal(B)) {
                    // For each production B -> γ
                    for (const auto& gamma : grammar.productions[B]) {
                        // Compute FIRST(βa)
                        string beta;
                        if (item.dotPosition + 1 < item.rhs.size()) {
                            for (size_t i = item.dotPosition + 1; i < item.rhs.size(); ++i) {
                                if (i > item.dotPosition + 1) beta += " ";
                                beta += item.rhs[i];
                            }
                        }
                        
                        // Add lookahead to beta for FIRST computation
                        if (!beta.empty()) beta += " ";
                        beta += item.lookahead;
                        
                        // Compute FIRST(βa)
                        bool nullable = false;
                        set<string> firstBeta = computeFirstForString(beta, grammar, nullable);
                        
                        // Add [B -> • γ, b] for each b in FIRST(βa)
                        for (const auto& b : firstBeta) {
                            LR1Item newItem(B, grammar.split(gamma), 0, b);
                            // FIX: check against both closureSet.items AND newItems
                            // to avoid spurious changed=true triggers
                            if (closureSet.items.find(newItem) == closureSet.items.end() &&
                                newItems.find(newItem) == newItems.end()) {
                                newItems.insert(newItem);
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
        
        closureSet.items.insert(newItems.begin(), newItems.end());
    }
    
    return closureSet;
}

// GOTO operation for LR(1)
LR1ItemSet CanonicalLR1Collection::goTo(const LR1ItemSet& I, const string& X, Grammar& grammar) {
    LR1ItemSet J;
    
    for (const auto& item : I.items) {
        if (item.dotPosition < item.rhs.size() && item.rhs[item.dotPosition] == X) {
            LR1Item newItem = item;
            newItem.dotPosition++;
            J.items.insert(newItem);
        }
    }
    
    return closure(J, grammar);
}

// Build canonical LR(1) collection
void CanonicalLR1Collection::buildCanonicalCollection(Grammar& grammar) {
    // Initialize with [S' -> • S, $]
    LR1ItemSet initial;
    vector<string> startRhs = grammar.productions[grammar.startSymbol];
    
    if (!startRhs.empty()) {
        initial.items.insert(LR1Item(grammar.startSymbol, grammar.split(startRhs[0]), 0, "$"));
    }
    
    LR1ItemSet I0 = closure(initial, grammar);
    states.push_back(I0);
    stateIndex[I0] = 0;

    bool changed = true;
    while (changed) {
        changed = false;
        size_t currentSize = states.size();
        
        for (size_t i = 0; i < currentSize; ++i) {
            // FIX: copy instead of reference — states.push_back() can reallocate
            // the vector, invalidating any reference/pointer into it. Using a
            // dangling reference then causes UB (garbage items, infinite loop).
            LR1ItemSet I = states[i];
            set<string> symbols;
            
            // Collect all symbols after the dot
            for (const auto& item : I.items) {
                if (item.dotPosition < item.rhs.size()) {
                    symbols.insert(item.rhs[item.dotPosition]);
                }
            }
            
            for (const auto& X : symbols) {
                LR1ItemSet gotoI = goTo(I, X, grammar);
                if (!gotoI.items.empty()) {
                    if (stateIndex.find(gotoI) == stateIndex.end()) {
                        int newIndex = states.size();
                        states.push_back(gotoI);
                        stateIndex[gotoI] = newIndex;
                        changed = true;
                    }
                    int targetIndex = stateIndex[gotoI];
                    transitions[{i, X}] = targetIndex;
                }
            }
        }
    }
}

// Print all states
void CanonicalLR1Collection::printStates() {
    for (size_t i = 0; i < states.size(); ++i) {
        cout << "I" << i << ":" << endl;
        cout << states[i].toString() << endl;
    }
}

// Build LR(1) parser
void LR1Parser::buildParser(const string& grammarFile) {
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

// Build action table for LR(1)
void LR1Parser::buildActionTable() {
    for (size_t i = 0; i < canonicalCollection.states.size(); ++i) {
        const LR1ItemSet& I = canonicalCollection.states[i];
        
        for (const auto& item : I.items) {
            if (item.dotPosition < item.rhs.size()) {
                // Shift action
                string a = item.rhs[item.dotPosition];
                if (grammar.isTerminal(a)) {
                    auto it = canonicalCollection.transitions.find({i, a});
                    if (it != canonicalCollection.transitions.end()) {
                        int j = it->second;
                        actionTable[{i, a}] = LR1Action(LR1_SHIFT, j);
                    }
                }
            } else {
                // Reduce action
                if (item.lhs == grammar.startSymbol) {
                    // Accept action
                    actionTable[{i, item.lookahead}] = LR1Action(LR1_ACCEPT, -1);
                } else {
                    // Reduce on specific lookahead
                    actionTable[{i, item.lookahead}] = LR1Action(LR1_REDUCE, getProductionIndex(item));
                }
            }
        }
    }
}

// Build goto table for LR(1)
void LR1Parser::buildGotoTable() {
    for (const auto& trans : canonicalCollection.transitions) {
        int i = trans.first.first;
        string X = trans.first.second;
        int j = trans.second;
        if (grammar.isNonTerminal(X)) {
            gotoTable[{i, X}] = j;
        }
    }
}

// Get production index
int LR1Parser::getProductionIndex(const LR1Item& item) {
    string rhsStr = joinRhs(item.rhs);
    for (size_t i = 0; i < productionList.size(); ++i) {
        if (productionList[i].first == item.lhs && joinRhs(productionList[i].second) == rhsStr) {
            return i;
        }
    }
    return -1;
}

// Join RHS symbols
string LR1Parser::joinRhs(const vector<string>& rhs) {
    string result;
    for (size_t i = 0; i < rhs.size(); ++i) {
        if (i > 0) result += " ";
        result += rhs[i];
    }
    return result;
}

// Get production by index
void LR1Parser::getProductionByIndex(int index, string& A, vector<string>& beta) {
    if (index >= 0 && index < (int)productionList.size()) {
        A = productionList[index].first;
        beta = productionList[index].second;
    }
}

// Parse input using LR(1) algorithm
bool LR1Parser::parse(const vector<string>& input) {
    Stack stack;
    stack.push("", 0); // initial state
    size_t inputIndex = 0;
    vector<string> inputWithEnd = input;
    inputWithEnd.push_back("$");

    while (true) {
        int s = stack.topState();
        string a = inputWithEnd[inputIndex];
        auto actionIt = actionTable.find({s, a});
        
        if (actionIt == actionTable.end()) {
            cout << "Error: No action for state " << s << " and symbol " << a << endl;
            return false;
        }
        
        LR1Action action = actionIt->second;

        if (action.type == LR1_SHIFT) {
            stack.push(a, action.value);
            inputIndex++;
        } else if (action.type == LR1_REDUCE) {
            // Find the production
            int prodIndex = action.value;
            string A;
            vector<string> beta;
            getProductionByIndex(prodIndex, A, beta);

            // Pop |beta| elements
            for (size_t i = 0; i < beta.size(); ++i) {
                stack.pop();
            }

            int t = stack.topState();
            auto gotoIt = gotoTable.find({t, A});
            if (gotoIt == gotoTable.end()) {
                cout << "Error: No goto for state " << t << " and non-terminal " << A << endl;
                return false;
            }
            int newState = gotoIt->second;
            stack.push(A, newState);
        } else if (action.type == LR1_ACCEPT) {
            cout << "Parsing successful!" << endl;
            return true;
        } else {
            cout << "Error: Invalid action" << endl;
            return false;
        }
    }
}

// Print parsing table
void LR1Parser::printParsingTable() {
    cout << "LR(1) Parsing Table:" << endl;
    // Print header
    cout << setw(5) << "State";
    for (const auto& term : grammar.terminals) {
        cout << setw(10) << term;
    }
    cout << setw(5) << "$";
    for (const auto& nt : grammar.nonTerminals) {
        cout << setw(10) << nt;
    }
    cout << endl;

    for (size_t i = 0; i < canonicalCollection.states.size(); ++i) {
        cout << setw(5) << i;
        for (const auto& term : grammar.terminals) {
            auto it = actionTable.find({i, term});
            if (it != actionTable.end()) {
                LR1Action a = it->second;
                if (a.type == LR1_SHIFT) cout << setw(10) << "s" + to_string(a.value);
                else if (a.type == LR1_REDUCE) cout << setw(10) << "r" + to_string(a.value);
                else cout << setw(10) << "";
            } else {
                cout << setw(10) << "";
            }
        }
        // $
        auto it = actionTable.find({i, "$"});
        if (it != actionTable.end()) {
            LR1Action a = it->second;
            if (a.type == LR1_ACCEPT) cout << setw(5) << "acc";
            else cout << setw(5) << "";
        } else {
            cout << setw(5) << "";
        }
        // GOTO
        for (const auto& nt : grammar.nonTerminals) {
            auto git = gotoTable.find({i, nt});
            if (git != gotoTable.end()) {
                cout << setw(10) << git->second;
            } else {
                cout << setw(10) << "";
            }
        }
        cout << endl;
    }
}

// Print parsing trace
void LR1Parser::printTrace(const vector<string>& input) {
    Stack stack;
    stack.push("", 0);
    size_t inputIndex = 0;
    vector<string> inputWithEnd = input;
    inputWithEnd.push_back("$");
    int step = 1;

    cout << "Parsing Trace:" << endl;
    cout << setw(5) << "Step" << setw(15) << "Stack" << setw(20) << "Input" << setw(15) << "Action" << endl;

    while (true) {
        // Print current state
        cout << setw(5) << step;
        cout << setw(15) << stack.toString();
        string remainingInput;
        for (size_t j = inputIndex; j < inputWithEnd.size(); ++j) {
            remainingInput += inputWithEnd[j] + " ";
        }
        cout << setw(20) << remainingInput;

        int s = stack.topState();
        string a = inputWithEnd[inputIndex];
        auto actionIt = actionTable.find({s, a});
        if (actionIt == actionTable.end()) {
            cout << setw(15) << "Error" << endl;
            return;
        }
        LR1Action action = actionIt->second;

        if (action.type == LR1_SHIFT) {
            cout << setw(15) << "Shift " + to_string(action.value) << endl;
            stack.push(a, action.value);
            inputIndex++;
        } else if (action.type == LR1_REDUCE) {
            string A;
            vector<string> beta;
            getProductionByIndex(action.value, A, beta);
            string prodStr = A + " -> " + joinRhs(beta);
            cout << setw(15) << "Reduce " + prodStr << endl;

            for (size_t i = 0; i < beta.size(); ++i) {
                stack.pop();
            }
            int t = stack.topState();
            auto gotoIt = gotoTable.find({t, A});
            int newState = gotoIt->second;
            stack.push(A, newState);
        } else if (action.type == LR1_ACCEPT) {
            cout << setw(15) << "Accept" << endl;
            return;
        } else {
            cout << setw(15) << "Error" << endl;
            return;
        }
        step++;
    }
}