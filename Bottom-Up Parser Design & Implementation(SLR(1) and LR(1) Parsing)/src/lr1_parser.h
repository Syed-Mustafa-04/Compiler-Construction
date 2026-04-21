#ifndef LR1_PARSER_H
#define LR1_PARSER_H

#include "grammar.h"
#include "items.h"
#include "slr_parser.h"
#include <vector>
#include <map>
#include <string>

class LR1Parser {
public:
    Grammar grammar;
    CanonicalLR1Collection canonicalCollection;
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
    int getProductionIndex(const LR1Item& item);
    std::string joinRhs(const std::vector<std::string>& rhs);
    void getProductionByIndex(int index, std::string& A, std::vector<std::string>& beta);
};

#endif
