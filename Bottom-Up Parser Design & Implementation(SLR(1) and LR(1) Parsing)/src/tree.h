#ifndef TREE_H
#define TREE_H

#include <string>
#include <vector>

struct ParseTreeNode {
    std::string symbol;
    std::vector<ParseTreeNode*> children;

    ParseTreeNode(const std::string& s) : symbol(s) {}
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

    void buildFromParsing(const std::vector<std::string>& actions);
    void print() const;
};

#endif