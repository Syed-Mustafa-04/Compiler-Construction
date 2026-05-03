#ifndef SLR_PARSER_H
#define SLR_PARSER_H

#include "grammar.h"
#include "items.h"
#include "tree.h"
#include <vector>
#include <map>
#include <string>
using namespace std;

enum ActionType { SHIFT, REDUCE, ACCEPT, ERROR };

struct Action {
    ActionType type;
    int value; // for SHIFT: state number, for REDUCE: production index

    Action(ActionType t = ERROR, int v = -1) : type(t), value(v) {}
};

class SLRParser {
public:
    Grammar grammar;
    CanonicalCollection canonicalCollection;
    map<pair<int, string>, Action> actionTable;
    map<pair<int, string>, int> gotoTable;
    vector<pair<string, vector<string>>> productionList;

    void buildParser(const string& grammarFile);
    void buildActionTable();
    void buildGotoTable();
    bool parse(const vector<string>& input);
    ParseTree parseWithTree(const vector<string>& input);
    void printParsingTable();
    void printTrace(const vector<string>& input);
private:
    int getProductionIndex(const LR0Item& item);
    string joinRhs(const vector<string>& rhs);
    void getProductionByIndex(int index, string& A, vector<string>& beta);
};

#endif
