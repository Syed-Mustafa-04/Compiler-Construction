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

#endif