#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <string>
#include <vector>
#include <set>
#include <map>
using namespace std;

class Grammar {
public:
    string startSymbol;
    set<string> nonTerminals;
    set<string> terminals;
    map<string, vector<string>> productions;
    map<string, set<string>> first;
    map<string, set<string>> follow;

    Grammar();
    void readFromFile(const string& filename);
    void parseProduction(const string& line);
    void augmentGrammar();
    void computeFirstSets();
    set<string> computeFirst(const string& alpha);
    void computeFollowSets();
    vector<string> split(const string& s);
    string join(vector<string>::iterator start, vector<string>::iterator end);
    bool isTerminal(const string& symbol);
    bool isNonTerminal(const string& symbol);
    void printAugmentedGrammar();
};

#endif
