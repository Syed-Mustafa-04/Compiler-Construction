#ifndef TREE_H
#define TREE_H

#include <string>
#include <vector>
using namespace std;

struct ParseTreeNode {
    string symbol;
    vector<ParseTreeNode*> children;

    ParseTreeNode(const string& s) : symbol(s) {}
    ~ParseTreeNode() {
        for (auto child : children) {
            delete child;
        }
    }

    void print(int depth = 0) const;
};

class ParseTree {
public:
    ParseTreeNode* root;

    ParseTree() : root(nullptr) {}
    ~ParseTree() { delete root; }

    void setRoot(ParseTreeNode* r) { root = r; }
    void buildFromParsing(const vector<string>& actions);
    void print() const;
    void printToFile(const string& filename) const;
};

#endif
