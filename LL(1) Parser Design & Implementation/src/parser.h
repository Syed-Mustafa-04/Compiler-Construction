#ifndef PARSER_H
#define PARSER_H

#include "grammar.h"
#include "first_follow.h"
#include "stack.h"
#include "tree.h"
#include <map>
#include <set>
#include <string>
#include <vector>
#include <memory>
#include <sstream>

using namespace std;

struct ParsingTableEntry {
    vector<vector<string>> productions;
    bool isConflict() const { return productions.size() > 1; }
};

class Parser {
public:
    Parser(const Grammar& g, const FirstFollow& ff);

    const ParsingTableEntry& getEntry(const string& nt,
                                       const string& terminal) const;

    bool isLL1() const { return conflictCount == 0; }
    int getConflictCount() const { return conflictCount; }
    int getErrorCount() const { return errorCount; }
    const vector<string>& getErrorMessages() const { return errorMessages; }
    void resetErrors() { errorCount = 0; errorMessages.clear(); }

    void display() const;
    void writeToFile(const string& filename) const;

    bool parse(const string& input);
    bool parseWithTrace(const string& input, const string& traceFile);

    const ParseTree& getParseTree() const { return lastParseTree; }
    void resetParseTree() { lastParseTree.clear(); }

private:
    const Grammar& gram;
    const FirstFollow& ff;
    map<string, ParsingTableEntry> table;
    set<string> tableTerminals;
    int conflictCount;
    int errorCount;
    vector<string> errorMessages;
    ParseTree lastParseTree;

    static string makeKey(const string& nt, const string& terminal) {
        return nt + ":" + terminal;
    }

    void addProduction(const string& nt, const string& terminal,
                       const vector<string>& production);

    static vector<string> tokenize(const string& input) {
        vector<string> tokens;
        istringstream iss(input);
        string token;
        while (iss >> token)
            tokens.push_back(token);
        if (tokens.empty() || tokens.back() != "$")
            tokens.push_back("$");
        return tokens;
    }

    bool runParser(const vector<string>& tokens,
                   ofstream* traceFile = nullptr);

    bool isTerminal(const string& symbol) const {
        return gram.terminals.find(symbol) != gram.terminals.end() || symbol == "$";
    }

    set<string> getSynchronizingSymbols(const string& nt) const {
        auto it = ff.followSets.find(nt);
        if (it != ff.followSets.end())
            return it->second;
        return set<string>();
    }

    void recordError(int step, [[maybe_unused]] const string& X,
                     [[maybe_unused]] const string& a, const string& errorMsg) {
        ++errorCount;
        ostringstream oss;
        oss << "Step " << step << ": [ERROR] " << errorMsg;
        errorMessages.push_back(oss.str());
    }

    static string productionToString(const string& lhs,
                                          const vector<string>& rhs) {
        string result = lhs + " -> ";
        for (size_t i = 0; i < rhs.size(); ++i) {
            if (i > 0) result += " ";
            result += rhs[i];
        }
        return result;
    }

    void printTable(ostream& out) const;
};

#endif

