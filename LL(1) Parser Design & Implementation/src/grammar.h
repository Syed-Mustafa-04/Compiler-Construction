#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>

using namespace std;

struct Production {
    string lhs;                    // left-hand side non-terminal
    vector<string> rhs;       // right-hand side symbols
};

// The full grammar object
class Grammar {
public:
    // Ordered list of non-terminals (order matters for indirect LR removal)
    vector<string>  nonTerminals;

    // Set of terminals
    set<string>     terminals;

    // Start symbol (first non-terminal seen)
    string               startSymbol;

    // All productions, keyed by LHS non-terminal
    map<string, vector<vector<string>>> productions;

    // ── I/O ──────────────────────────────────────────────────
    bool loadFromFile(const string& filename);

    // ── Transformations ──────────────────────────────────────
    void applyLeftFactoring();
    void removeLeftRecursion();

    // ── Display ──────────────────────────────────────────────
    void display(const string& title = "") const;
    void writeToFile(const string& filename, const string& title = "") const;

    // ── Helpers ──────────────────────────────────────────────
    bool isNonTerminal(const string& sym) const;
    bool isTerminal(const string& sym) const;
    bool isEpsilon(const string& sym) const;

private:
    // Left-factoring internals
    void factorNonTerminal(const string& nt);
    string freshNT(const string& base) const;

    // Left-recursion internals
    void eliminateDirectLR(const string& nt);
    void substituteProductions(const string& ai, const string& aj);

    // Parsing helpers
    vector<string> tokenizeLine(const string& line) const;
    vector<string> splitAlternatives(const string& rhs) const;
};

#endif 
