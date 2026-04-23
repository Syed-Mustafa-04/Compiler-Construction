#include "slr_parser.h"
#include "stack.h"
#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;

void SLRParser::buildParser(const string& grammarFile) {
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
                string a = item.rhs[item.dotPosition];
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
        string X = trans.first.second;
        int j = trans.second;
        if (grammar.isNonTerminal(X)) {
            gotoTable[{i, X}] = j;
        }
    }
}

int SLRParser::getProductionIndex(const LR0Item& item) {
    string rhsStr = joinRhs(item.rhs);
    for (size_t i = 0; i < productionList.size(); ++i) {
        if (productionList[i].first == item.lhs && joinRhs(productionList[i].second) == rhsStr) {
            return i;
        }
    }
    return -1;
}

string SLRParser::joinRhs(const vector<string>& rhs) {
    string result;
    for (size_t i = 0; i < rhs.size(); ++i) {
        if (i > 0) result += " ";
        result += rhs[i];
    }
    return result;
}

bool SLRParser::parse(const vector<string>& input) {
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
        Action action = actionIt->second;

        if (action.type == SHIFT) {
            stack.push(a, action.value);
            inputIndex++;
        } else if (action.type == REDUCE) {
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
        } else if (action.type == ACCEPT) {
            cout << "Parsing successful!" << endl;
            return true;
        } else {
            cout << "Error: Invalid action" << endl;
            return false;
        }
    }
}

ParseTree SLRParser::parseWithTree(const vector<string>& input) {
    ParseTree tree;
    Stack stack;
    stack.push("", 0);
    size_t inputIndex = 0;
    vector<string> inputWithEnd = input;
    inputWithEnd.push_back("$");

    while (true) {
        int s = stack.topState();
        string a = inputWithEnd[inputIndex];
        auto actionIt = actionTable.find({s, a});
        if (actionIt == actionTable.end()) {
            return tree; // return empty tree on error
        }
        Action action = actionIt->second;

        if (action.type == SHIFT) {
            // Create leaf node for terminal
            ParseTreeNode* leafNode = new ParseTreeNode(a);
            stack.push(a, action.value, leafNode);
            inputIndex++;
        } else if (action.type == REDUCE) {
            int prodIndex = action.value;
            string A;
            vector<string> beta;
            getProductionByIndex(prodIndex, A, beta);

            // Create internal node for non-terminal
            ParseTreeNode* nonTermNode = new ParseTreeNode(A + " -> " + joinRhs(beta));

            // Collect children from stack
            vector<ParseTreeNode*> children;
            size_t popCount = beta.size();
            for (size_t i = 0; i < popCount; ++i) {
                ParseTreeNode* child = stack.getTreeNode(stack.elements.size() - 1 - i);
                if (child) {
                    children.insert(children.begin(), child);
                }
            }

            // Attach children to internal node
            for (auto child : children) {
                nonTermNode->children.push_back(child);
            }

            // Pop elements from stack
            for (size_t i = 0; i < popCount; ++i) {
                stack.pop();
            }

            int t = stack.topState();
            auto gotoIt = gotoTable.find({t, A});
            if (gotoIt == gotoTable.end()) {
                return tree;
            }
            int newState = gotoIt->second;
            stack.push(A, newState, nonTermNode);
        } else if (action.type == ACCEPT) {
            // Set root to the non-terminal on top of stack
            if (!stack.elements.empty() && stack.elements.size() > 1) {
                tree.setRoot(stack.treeNodes.back());
            }
            return tree;
        } else {
            return tree;
        }
    }
}

void SLRParser::getProductionByIndex(int index, string& A, vector<string>& beta) {
    if (index >= 0 && index < (int)productionList.size()) {
        A = productionList[index].first;
        beta = productionList[index].second;
    }
}

void SLRParser::printParsingTable() {
    cout << "SLR(1) Parsing Table:" << endl;
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
                Action a = it->second;
                if (a.type == SHIFT) cout << setw(10) << "s" + to_string(a.value);
                else if (a.type == REDUCE) cout << setw(10) << "r" + to_string(a.value);
                else cout << setw(10) << "";
            } else {
                cout << setw(10) << "";
            }
        }
        // $
        auto it = actionTable.find({i, "$"});
        if (it != actionTable.end()) {
            Action a = it->second;
            if (a.type == ACCEPT) cout << setw(5) << "acc";
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

void SLRParser::printTrace(const vector<string>& input) {
    // Similar to parse but print steps
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
        Action action = actionIt->second;

        if (action.type == SHIFT) {
            cout << setw(15) << "Shift " + to_string(action.value) << endl;
            stack.push(a, action.value);
            inputIndex++;
        } else if (action.type == REDUCE) {
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
        } else if (action.type == ACCEPT) {
            cout << setw(15) << "Accept" << endl;
            return;
        } else {
            cout << setw(15) << "Error" << endl;
            return;
        }
        step++;
    }
}
