#ifndef STACK_H
#define STACK_H

#include <vector>
#include <string>
using namespace std;

// Forward declaration
class ParseTreeNode;

class Stack {
public:
    vector<pair<string, int>> elements; // symbol, state
    vector<ParseTreeNode*> treeNodes;   // corresponding tree nodes (parallel stack)

    void push(const string& symbol, int state);
    void push(const string& symbol, int state, ParseTreeNode* node);
    void pop();
    int topState() const;
    string topSymbol() const;
    bool empty() const;
    string toString() const;
    
    ParseTreeNode* getTreeNode(size_t index) const;
    void setTreeNode(size_t index, ParseTreeNode* node);
};

#endif
