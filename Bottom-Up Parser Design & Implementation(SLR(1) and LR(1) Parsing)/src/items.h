#ifndef ITEMS_H
#define ITEMS_H

#include <string>
#include <vector>
#include <set>
#include <map>
#include "grammar.h"
using namespace std;

struct LR0Item 
{
    string lhs;
    vector<string> rhs;
    size_t dotPosition;

    LR0Item(string l, vector<string> r, size_t d) : lhs(l), rhs(r), dotPosition(d) {}

    bool operator==(const LR0Item& other) const {
        return lhs == other.lhs && rhs == other.rhs && dotPosition == other.dotPosition;
    }

    bool operator<(const LR0Item& other) const {
        if (lhs != other.lhs) return lhs < other.lhs;
        if (rhs != other.rhs) return rhs < other.rhs;
        return dotPosition < other.dotPosition;
    }

    string toString() const;
};

class ItemSet {
public:
    set<LR0Item> items;

    ItemSet() {}
    ItemSet(const set<LR0Item>& i) : items(i) {}

    bool operator==(const ItemSet& other) const {
        return items == other.items;
    }

    bool operator<(const ItemSet& other) const {
        return items < other.items;
    }

    string toString() const;
};

class CanonicalCollection {
public:
    vector<ItemSet> states;
    map<ItemSet, int> stateIndex;
    map<pair<int, string>, int> transitions;

    void buildCanonicalCollection(Grammar& grammar);
    ItemSet closure(const ItemSet& I, Grammar& grammar);
    ItemSet goTo(const ItemSet& I, const string& X, Grammar& grammar);
    void printStates();
};

#endif
