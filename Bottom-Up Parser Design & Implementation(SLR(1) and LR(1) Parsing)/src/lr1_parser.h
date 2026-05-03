#ifndef LR1_PARSER_H
#define LR1_PARSER_H

#include "grammar.h"
#include <vector>
#include <map>
#include <string>
#include <set>

// LR(1) Item: [A -> α • β, a] where a is a lookahead terminal
struct LR1Item {
    std::string lhs;
    std::vector<std::string> rhs;
    size_t dotPosition;
    std::string lookahead;

    LR1Item(std::string l, std::vector<std::string> r, size_t d, std::string la)
        : lhs(l), rhs(r), dotPosition(d), lookahead(la) {}

    bool operator==(const LR1Item& other) const {
        return lhs == other.lhs && rhs == other.rhs &&
               dotPosition == other.dotPosition && lookahead == other.lookahead;
    }

    bool operator<(const LR1Item& other) const {
        if (lhs != other.lhs) return lhs < other.lhs;
        if (rhs != other.rhs) return rhs < other.rhs;
        if (dotPosition != other.dotPosition) return dotPosition < other.dotPosition;
        return lookahead < other.lookahead;
    }

    std::string toString() const;
};

class LR1ItemSet {
public:
    std::set<LR1Item> items;

    LR1ItemSet() {}
    LR1ItemSet(const std::set<LR1Item>& i) : items(i) {}

    bool operator==(const LR1ItemSet& other) const {
        return items == other.items;
    }

    bool operator<(const LR1ItemSet& other) const {
        return items < other.items;
    }

    std::string toString() const;
};

class CanonicalLR1Collection {
public:
    std::vector<LR1ItemSet> states;
    std::map<LR1ItemSet, int> stateIndex;
    std::map<std::pair<int, std::string>, int> transitions;

    void buildCanonicalCollection(Grammar& grammar);
    LR1ItemSet closure(const LR1ItemSet& I, Grammar& grammar);
    LR1ItemSet goTo(const LR1ItemSet& I, const std::string& X, Grammar& grammar);
    void printStates();
};

enum LR1ActionType { LR1_SHIFT, LR1_REDUCE, LR1_ACCEPT, LR1_ERROR };

struct LR1Action {
    LR1ActionType type;
    int value; // for LR1_SHIFT: state number, for LR1_REDUCE: production index

    LR1Action(LR1ActionType t = LR1_ERROR, int v = -1) : type(t), value(v) {}
};

class LR1Parser {
public:
    Grammar grammar;
    CanonicalLR1Collection canonicalCollection;
    std::map<std::pair<int, std::string>, LR1Action> actionTable;
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
