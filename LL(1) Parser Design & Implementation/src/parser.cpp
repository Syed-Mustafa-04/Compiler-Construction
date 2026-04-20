#include "parser.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace std;

Parser::Parser(const Grammar& g, const FirstFollow& ff)
    : gram(g), ff(ff), conflictCount(0), errorCount(0) {
    for (const auto& t : gram.terminals)
        tableTerminals.insert(t);
    tableTerminals.insert("$");

    for (const auto& nt : gram.nonTerminals) {
        auto prodIt = gram.productions.find(nt);
        if (prodIt == gram.productions.end()) continue;

        for (const auto& production : prodIt->second) {
            std::set<std::string> firstAlpha = ff.firstOfSequence(production);

            for (const auto& terminal : firstAlpha) {
                if (terminal != "epsilon")
                    addProduction(nt, terminal, production);
            }

            if (firstAlpha.count("epsilon")) {
                auto followIt = ff.followSets.find(nt);
                if (followIt != ff.followSets.end()) {
                    for (const auto& terminal : followIt->second)
                        addProduction(nt, terminal, production);
                }
            }
        }
    }
}

void Parser::addProduction(const std::string& nt, const std::string& terminal,
                           const std::vector<std::string>& production) {
    std::string key = makeKey(nt, terminal);
    auto& entry = table[key];
    bool alreadyExists = std::any_of(entry.productions.begin(), entry.productions.end(),
                                     [&](const auto& p) { return p == production; });
    if (!alreadyExists) {
        if (!entry.productions.empty())
            conflictCount++;
        entry.productions.push_back(production);
    }
}

const ParsingTableEntry& Parser::getEntry(const std::string& nt,
                                           const std::string& terminal) const {
    std::string key = makeKey(nt, terminal);
    auto it = table.find(key);
    if (it != table.end())
        return it->second;
    static const ParsingTableEntry empty;
    return empty;
}

void Parser::printTable(std::ostream& out) const {
    cout << "\n" << std::string(80, '=') << "\n";
    cout << "  Task 1.6  LL(1) Parsing Table Construction\n";
    cout << std::string(80, '=') << "\n\n";
    if (isLL1())
        cout << "  Grammar IS LL(1)  (No conflicts in parsing table)\n";
    else
        out << "  Grammar IS NOT LL(1)  (Conflicts found: " << conflictCount << ")\n";
    out << "\n";

    std::vector<std::string> terminalList(tableTerminals.begin(), tableTerminals.end());
    std::sort(terminalList.begin(), terminalList.end(),
              [](const std::string& a, const std::string& b) {
                  return (a != "$" && (b == "$" || a < b));
              });

    int ntColWidth = 15;
    for (const auto& nt : gram.nonTerminals)
        ntColWidth = std::max(ntColWidth, (int)nt.size() + 2);
    int cellWidth = 25;

    out << "  " << std::left << std::setw(ntColWidth) << "Non-Terminal";
    for (const auto& term : terminalList)
        out << "| " << std::left << std::setw(cellWidth) << term << " ";
    out << "\n" << "  " << std::string(ntColWidth, '-');
    for (size_t i = 0; i < terminalList.size(); ++i)
        out << "+" << std::string(cellWidth + 2, '-');
    out << "\n";

    for (const auto& nt : gram.nonTerminals) {
        out << "  " << std::left << std::setw(ntColWidth) << nt;
        for (const auto& term : terminalList) {
            const ParsingTableEntry& entry = getEntry(nt, term);
            out << "| ";
            if (entry.productions.empty())
                out << std::left << std::setw(cellWidth) << "error";
            else if (entry.productions.size() == 1) {
                std::string prod = productionToString(nt, entry.productions[0]);
                if (prod.size() > (size_t)cellWidth - 2)
                    prod = prod.substr(0, cellWidth - 5) + "...";
                out << std::left << std::setw(cellWidth) << prod;
            } else
                out << std::left << std::setw(cellWidth) << "[CONFLICT]";
            out << " ";
        }
        out << "\n";
    }

    out << "  " << std::string(76, '-') << "\n\n  Summary:\n";
    out << "    Total Non-Terminals: " << gram.nonTerminals.size() << "\n";
    out << "    Total Terminals + $: " << tableTerminals.size() << "\n";
    out << "    Table Size: " << gram.nonTerminals.size() << " × " << tableTerminals.size() << "\n";
    out << "    Non-empty Entries: " << table.size() << "\n";
    out << "    Conflicts: " << conflictCount << "\n";
    out << "    LL(1) Status: " << (isLL1() ? "YES" : "NO") << "\n";
    out << std::string(80, '=') << "\n";
}

void Parser::display() const {
    printTable(std::cout);
}

void Parser::writeToFile(const std::string& filename) const {
    std::ofstream fout(filename, std::ios::app);
    if (!fout.is_open()) {
        std::cerr << "[ERROR] Cannot open output file: " << filename << "\n";
        return;
    }
    printTable(fout);
    fout.close();
}

bool Parser::parse(const std::string& input) {
    std::vector<std::string> tokens = Parser::tokenize(input);
    return runParser(tokens, nullptr);
}

bool Parser::parseWithTrace(const std::string& input, const std::string& traceFile) {
    std::vector<std::string> tokens = Parser::tokenize(input);
    if (traceFile.empty())
        return runParser(tokens, nullptr);

    std::ofstream fout(traceFile);
    if (!fout.is_open()) {
        std::cerr << "Error: Could not open trace file: " << traceFile << "\n";
        return false;
    }

    fout << "========== LL(1) Parsing Trace ==========\n";
    fout << "Input: " << input << "\n";
    fout << "Start Symbol: " << gram.startSymbol << "\n";
    fout << std::string(40, '=') << "\n\n";

    bool result = runParser(tokens, &fout);

    fout << "\n" << std::string(40, '=') << "\n";
    fout << (result ? "RESULT: ACCEPTED\n" : "RESULT: REJECTED\n");
    fout << std::string(40, '=') << "\n";
    fout.close();
    return result;
}

bool Parser::runParser(const std::vector<std::string>& tokens,
                       std::ofstream* traceFile) {
    Stack stack;
    stack.initForParsing(gram.startSymbol);

    int inputPtr = 0;
    int step = 0;

    lastParseTree.clear();
    auto rootNode = ParseTree::createNode(gram.startSymbol);
    lastParseTree.setRoot(rootNode);

    stack.clear();
    stack.push("$");
    stack.pushWithTreeNode(gram.startSymbol, rootNode);

    if (traceFile) {
        *traceFile << "Step | Stack                          | Input         | Action\n";
        *traceFile << std::string(80, '-') << "\n";
    }

    while (true) {
        step++;

        if (traceFile) {
            std::string stackStr = stack.toString();
            if (stackStr.length() > 30)
                stackStr = stackStr.substr(0, 27) + "...";

            std::string inputStr = "";
            for (int i = inputPtr; i < (int)tokens.size() && i < inputPtr + 3; ++i) {
                if (i > inputPtr) inputStr += " ";
                inputStr += tokens[i];
            }

            *traceFile << std::setw(4) << step << " | ";
            *traceFile << std::left << std::setw(30) << stackStr << " | ";
            *traceFile << std::left << std::setw(13) << inputStr << " | ";
        }

        if (stack.isEmpty())
            return false;

        std::string X = stack.top();
        std::string a = (inputPtr < (int)tokens.size()) ? tokens[inputPtr] : "$";

        if (X == "$" && a == "$") {
            if (traceFile)
                *traceFile << "Accept\n";
            return true;
        }

        if (isTerminal(X)) {
            if (X == a) {
                if (traceFile)
                    *traceFile << "Match '" << X << "'\n";
                stack.pop();
                inputPtr++;
            } else {
                std::string errorMsg = "Expected '" + X + "' but found '" + a + "'";
                recordError(step, X, a, errorMsg);
                if (traceFile) {
                    *traceFile << "ERROR: " << errorMsg << "\n";
                    *traceFile << "   [Recovery] Skipping '" << a << "'\n";
                }
                stack.pop();
                if (inputPtr < (int)tokens.size())
                    inputPtr++;
            }
        } else {
            const ParsingTableEntry& entry = getEntry(X, a);

            if (entry.productions.empty()) {
                std::string errorMsg = "No entry M[" + X + ", " + a + "]";
                recordError(step, X, a, errorMsg);
                if (traceFile)
                    *traceFile << "ERROR: " << errorMsg << "\n";
                if (inputPtr < (int)tokens.size()) {
                    if (traceFile)
                        *traceFile << "   [Recovery] Skipping '" << a << "'\n";
                    inputPtr++;
                } else
                    return false;
                continue;
            }

            if (entry.isConflict()) {
                std::string errorMsg = "Conflict in M[" + X + ", " + a + "]";
                recordError(step, X, a, errorMsg);
                if (traceFile)
                    *traceFile << "ERROR: " << errorMsg << "\n";
                if (inputPtr < (int)tokens.size())
                    inputPtr++;
                continue;
            }

            const std::vector<std::string>& production = entry.productions[0];

            if (traceFile) {
                *traceFile << "Expand " << X << " -> ";
                if (production.size() == 1 && production[0] == "epsilon")
                    *traceFile << "epsilon";
                else
                    for (size_t i = 0; i < production.size(); ++i) {
                        if (i > 0) *traceFile << " ";
                        *traceFile << production[i];
                    }
                *traceFile << "\n";
            }

            auto parentTreeNode = stack.popTreeNode();
            stack.pop();

            if (production.size() == 1 && production[0] == "epsilon") {
                if (parentTreeNode) {
                    auto epsilonNode = ParseTree::createNode("epsilon");
                    parentTreeNode->addChild(epsilonNode);
                }
            } else {
                std::vector<std::string> symbolsToStack;
                std::vector<std::shared_ptr<TreeNode>> nodesToStack;

                for (const auto& symbol : production) {
                    auto childNode = ParseTree::createNode(symbol);
                    if (parentTreeNode)
                        parentTreeNode->addChild(childNode);
                    symbolsToStack.push_back(symbol);
                    nodesToStack.push_back(childNode);
                }

                stack.pushProductionWithTreeNodes(symbolsToStack, nodesToStack);
            }
        }

        if (step > 10000)
            return false;
    }
}
