#include "tree.h"
#include <iostream>
#include <fstream>
#include <functional>
using namespace std;

void ParseTreeNode::print(int depth) const {
    for (int i = 0; i < depth; ++i) cout << "  ";
    cout << symbol << endl;
    for (const auto& child : children) {
        child->print(depth + 1);
    }
}

void ParseTree::print() const {
    if (root) {
        root->print();
    }
}

void ParseTree::printToFile(const string& filename) const {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }
    
    function<void(const ParseTreeNode*, int)> printNode = 
        [&](const ParseTreeNode* node, int depth) {
            if (!node) return;
            for (int i = 0; i < depth; ++i) file << "  ";
            file << node->symbol << endl;
            for (const auto& child : node->children) {
                printNode(child, depth + 1);
            }
        };
    
    if (root) {
        file << "Parse Tree:" << endl;
        printNode(root, 0);
    } else {
        file << "No parse tree generated." << endl;
    }
    file.close();
}

void ParseTree::buildFromParsing(const vector<string>& actions) {
    (void)actions; // Parameter used in future enhancements
    // Tree is now built directly during parsing via buildTreeWithReduction
}
