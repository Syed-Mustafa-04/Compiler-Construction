#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <string>
#include <vector>
#include <set>
#include <map>

class Grammar {
public:
    std::string startSymbol;
    std::set<std::string> nonTerminals;
    std::set<std::string> terminals;
    std::map<std::string, std::vector<std::string>> productions;
    std::map<std::string, std::set<std::string>> first;
    std::map<std::string, std::set<std::string>> follow;

    Grammar();
    void readFromFile(const std::string& filename);
    void parseProduction(const std::string& line);
    void augmentGrammar();
    void computeFirstSets();
    std::set<std::string> computeFirst(const std::string& alpha);
    void computeFollowSets();
    std::vector<std::string> split(const std::string& s);
    std::string join(std::vector<std::string>::iterator start, std::vector<std::string>::iterator end);
    bool isTerminal(const std::string& symbol);
    bool isNonTerminal(const std::string& symbol);
    void printAugmentedGrammar();
};

#endif