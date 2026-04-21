#include "tree.h"
#include <iostream>

void ParseTreeNode::print(int depth) const {
    for (int i = 0; i < depth; ++i) std::cout << "  ";
    std::cout << symbol << std::endl;
    for (const auto& child : children) {
        child->print(depth + 1);
    }
}

void ParseTree::print() const {
    if (root) {
        root->print();
    }
}

void ParseTree::buildFromParsing(const std::vector<std::string>& actions) {
    // Placeholder: need to implement proper tree building
    root = new ParseTreeNode("Start");
}