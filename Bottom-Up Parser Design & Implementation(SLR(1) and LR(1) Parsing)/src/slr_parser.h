#ifndef SLR_PARSER_H
#define SLR_PARSER_H

#include "grammar.h"
#include "items.h"
#include <vector>
#include <map>
#include <string>

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
    std::map<std::pair<int, std::string>, Action> actionTable;
    std::map<std::pair<int, std::string>, int> gotoTable;
    std::vector<std::pair<std::string, std::vector<std::string>>> productionList;

    void buildParser(const std::string& grammarFile);
    void buildActionTable();
    void buildGotoTable();
    bool parse(const std::vector<std::string>& input);
    void printParsingTable();
    void printTrace(const std::vector<std::string>& input);
private:
    int getProductionIndex(const LR0Item& item);
    std::string joinRhs(const std::vector<std::string>& rhs);
    void getProductionByIndex(int index, std::string& A, std::vector<std::string>& beta);
};

#endif