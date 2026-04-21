#include "items.h"
#include <iostream>
#include <algorithm>

std::string LR0Item::toString() const {
    std::string result = lhs + " -> ";
    for (size_t i = 0; i < rhs.size(); ++i) {
        if (i == dotPosition) result += "• ";
        result += rhs[i] + " ";
    }
    if (dotPosition == rhs.size()) result += "•";
    return result;
}

std::string ItemSet::toString() const {
    std::string result;
    for (const auto& item : items) {
        result += item.toString() + "\n";
    }
    return result;
}

void CanonicalCollection::buildCanonicalCollection(Grammar& grammar) {
    ItemSet initial;
    std::vector<std::string> startRhs = grammar.productions[grammar.startSymbol];
    if (!startRhs.empty()) {
        initial.items.insert(LR0Item(grammar.startSymbol, grammar.split(startRhs[0]), 0));
    }
    ItemSet I0 = closure(initial, grammar);
    states.push_back(I0);
    stateIndex[I0] = 0;

    bool changed = true;
    while (changed) {
        changed = false;
        size_t currentSize = states.size();
        for (size_t i = 0; i < currentSize; ++i) {
            const ItemSet& I = states[i];
            std::set<std::string> symbols;
            for (const auto& item : I.items) {
                if (item.dotPosition < item.rhs.size()) {
                    symbols.insert(item.rhs[item.dotPosition]);
                }
            }
            for (const auto& X : symbols) {
                ItemSet gotoI = goTo(I, X, grammar);
                if (!gotoI.items.empty()) {
                    if (stateIndex.find(gotoI) == stateIndex.end()) {
                        int newIndex = states.size();
                        states.push_back(gotoI);
                        stateIndex[gotoI] = newIndex;
                        changed = true;
                    }
                    int targetIndex = stateIndex[gotoI];
                    transitions[{i, X}] = targetIndex;
                }
            }
        }
    }
}

ItemSet CanonicalCollection::closure(const ItemSet& I, Grammar& grammar) {
    ItemSet closureSet = I;
    bool changed = true;
    while (changed) {
        changed = false;
        std::set<LR0Item> newItems;
        for (const auto& item : closureSet.items) {
            if (item.dotPosition < item.rhs.size()) {
                std::string B = item.rhs[item.dotPosition];
                if (grammar.isNonTerminal(B)) {
                    for (const auto& prod : grammar.productions[B]) {
                        LR0Item newItem(B, grammar.split(prod), 0);
                        if (closureSet.items.find(newItem) == closureSet.items.end()) {
                            newItems.insert(newItem);
                            changed = true;
                        }
                    }
                }
            }
        }
        closureSet.items.insert(newItems.begin(), newItems.end());
    }
    return closureSet;
}

ItemSet CanonicalCollection::goTo(const ItemSet& I, const std::string& X, Grammar& grammar) {
    ItemSet J;
    for (const auto& item : I.items) {
        if (item.dotPosition < item.rhs.size() && item.rhs[item.dotPosition] == X) {
            LR0Item newItem = item;
            newItem.dotPosition++;
            J.items.insert(newItem);
        }
    }
    return closure(J, grammar);
}

void CanonicalCollection::printStates() {
    for (size_t i = 0; i < states.size(); ++i) {
        std::cout << "I" << i << ":" << std::endl;
        std::cout << states[i].toString() << std::endl;
    }
}