#include "stack.h"
#include "tree.h"
#include <sstream>
using namespace std;

void Stack::push(const string& symbol, int state) {
    elements.push_back({symbol, state});
    treeNodes.push_back(nullptr);
}

void Stack::push(const string& symbol, int state, ParseTreeNode* node) {
    elements.push_back({symbol, state});
    treeNodes.push_back(node);
}

void Stack::pop() {
    if (!elements.empty()) {
        // Note: we don't delete the tree node as it's still in use by parent
        elements.pop_back();
        if (!treeNodes.empty()) {
            treeNodes.pop_back();
        }
    }
}

int Stack::topState() const {
    return elements.empty() ? -1 : elements.back().second;
}

string Stack::topSymbol() const {
    return elements.empty() ? "" : elements.back().first;
}

bool Stack::empty() const {
    return elements.empty();
}

string Stack::toString() const {
    stringstream ss;
    for (const auto& e : elements) {
        ss << e.first << e.second << " ";
    }
    return ss.str();
}

ParseTreeNode* Stack::getTreeNode(size_t index) const {
    if (index < treeNodes.size()) {
        return treeNodes[index];
    }
    return nullptr;
}

void Stack::setTreeNode(size_t index, ParseTreeNode* node) {
    if (index < treeNodes.size()) {
        treeNodes[index] = node;
    }
}
