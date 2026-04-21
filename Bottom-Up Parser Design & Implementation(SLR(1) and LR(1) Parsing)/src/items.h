#ifndef ITEMS_H
#define ITEMS_H

#include <string>
#include <vector>
#include <set>
#include <map>
#include "grammar.h"

struct LR0Item 
{
    std::string lhs;
    std::vector<std::string> rhs;
    size_t dotPosition;

    LR0Item(std::string l, std::vector<std::string> r, size_t d) : lhs(l), rhs(r), dotPosition(d) {}

    bool operator==(const LR0Item& other) const {
        return lhs == other.lhs && rhs == other.rhs && dotPosition == other.dotPosition;
    }

    bool operator<(const LR0Item& other) const {
        if (lhs != other.lhs) return lhs < other.lhs;
        if (rhs != other.rhs) return rhs < other.rhs;
        return dotPosition < other.dotPosition;
    }

    std::string toString() const;
};

class ItemSet {
public:
    std::set<LR0Item> items;

    ItemSet() {}
    ItemSet(const std::set<LR0Item>& i) : items(i) {}

    bool operator==(const ItemSet& other) const {
        return items == other.items;
    }

    bool operator<(const ItemSet& other) const {
        return items < other.items;
    }

    std::string toString() const;
};

// LR(1) item: [A -> α • β, a] where a is a lookahead terminal
struct LR1Item
{
    std::string lhs;
    std::vector<std::string> rhs;
    size_t dotPosition;
    std::string lookahead;

    LR1Item(std::string l, std::vector<std::string> r, size_t d, std::string la) 
        : lhs(l), rhs(r), dotPosition(d), lookahead(la) {}

    bool operator==(const LR1Item& other) const {
        return lhs == other.lhs && rhs == other.rhs && dotPosition == other.dotPosition && lookahead == other.lookahead;
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

class CanonicalCollection {
public:
    std::vector<ItemSet> states;
    std::map<ItemSet, int> stateIndex;
    std::map<std::pair<int, std::string>, int> transitions;

    void buildCanonicalCollection(Grammar& grammar);
    ItemSet closure(const ItemSet& I, Grammar& grammar);
    ItemSet goTo(const ItemSet& I, const std::string& X, Grammar& grammar);
    void printStates();
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

#endif
