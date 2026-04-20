#include "tree.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

ParseTree::ParseTree() : root(nullptr) {}

ParseTree::~ParseTree() {
    clear();
}


void ParseTree::clear() {
    root = nullptr;
}

//  display() – ASCII art tree to stdout


void ParseTree::display() const {
    if (!root) {
        cout << "[INFO] Parse tree is empty.\n";
        return;
    }
    
    cout << "\n" << string(80, '=') << "\n";
    cout << "  Task 2.5  –  Parse Tree (ASCII Art)\n";
    cout << string(80, '=') << "\n\n";
    
    displayHelper(root, "", true, cout);
    
    cout << "\n";
}


//  displayHelper() – Recursive ASCII tree display


void ParseTree::displayHelper(shared_ptr<TreeNode> node,
                              const string& prefix,
                              bool isLast,
                              ostream& out) const {
    if (!node) return;
    
    out << prefix;
    out << (isLast ? "└── " : "├── ");
    out << node->symbol << "\n";
    
    for (size_t i = 0; i < node->children.size(); ++i) {
        bool childIsLast = (i == node->children.size() - 1);
        string newPrefix = prefix + (isLast ? "    " : "│   ");
        displayHelper(node->children[i], newPrefix, childIsLast, out);
    }
}



//  displayIndented() – Indented format to stdout
void ParseTree::displayIndented() const {
    if (!root) {
        cout << "[INFO] Parse tree is empty.\n";
        return;
    }
    
    cout << "\n" << string(80, '=') << "\n";
    cout << "  Task 2.5  –  Parse Tree (Indented Format)\n";
    cout << string(80, '=') << "\n\n";
    
    displayIndentedHelper(root, 0, cout);
    
    cout << "\n";
}

//  displayIndentedHelper() – Recursive indented display
void ParseTree::displayIndentedHelper(shared_ptr<TreeNode> node,
                                      int depth,
                                      ostream& out) const {
    if (!node) return;
    
    out << string(depth * 2, ' ');
    
    if (node->isLeaf()) {
        out << "• " << node->symbol << "\n";
    } else {
        out << "▪ " << node->symbol << "\n";
        for (auto& child : node->children) {
            displayIndentedHelper(child, depth + 1, out);
        }
    }
}


//  writeToFile() – Write tree in ASCII art format only
void ParseTree::writeToFile(const string& filename) const {
    if (!root) {
        cerr << "[ERROR] Parse tree is empty. Cannot write to file.\n";
        return;
    }
    
    ofstream fout(filename, ios::app);
    if (!fout.is_open()) {
        cerr << "[ERROR] Cannot open file: " << filename << "\n";
        return;
    }
    
    // ── ASCII Art Format Only ──────────────────────────────────
    fout << "\n" << string(80, '=') << "\n";
    fout << "  Parse Tree (ASCII Art Format)\n";
    fout << string(80, '=') << "\n\n";
    
    displayHelper(root, "", true, fout);
    
    fout << "\n";
    fout << string(80, '=') << "\n";
    
    fout.close();
}

//  writeIndentedToFile() – Write tree in indented format only

void ParseTree::writeIndentedToFile(const string& filename, bool append) const {
    if (!root) {
        cerr << "[ERROR] Parse tree is empty. Cannot write to file.\n";
        return;
    }
    
    ios_base::openmode mode = append ? ios::app : ios::trunc;
    ofstream fout(filename, mode);
    if (!fout.is_open()) {
        cerr << "[ERROR] Cannot open file: " << filename << "\n";
        return;
    }
    
    fout << "\n" << string(80, '=') << "\n";
    fout << "  Parse Tree (Indented Format)\n";
    fout << string(80, '=') << "\n\n";
    
    displayIndentedHelper(root, 0, fout);
    
    fout << "\n" << string(80, '=') << "\n";
    
    fout.close();
}


//  preorderTraversal() – Visit: Root, Left, Right


vector<string> ParseTree::preorderTraversal() const {
    vector<string> result;
    preorderHelper(root, result);
    return result;
}

void ParseTree::preorderHelper(shared_ptr<TreeNode> node,
                               vector<string>& result) const {
    if (!node) return;
    
    result.push_back(node->symbol);
    for (auto& child : node->children) {
        preorderHelper(child, result);
    }
}

//  postorderTraversal() – Visit: Left, Right, Root

vector<string> ParseTree::postorderTraversal() const {
    vector<string> result;
    postorderHelper(root, result);
    return result;
}

void ParseTree::postorderHelper(shared_ptr<TreeNode> node,
                                vector<string>& result) const {
    if (!node) return;
    
    for (auto& child : node->children) {
        postorderHelper(child, result);
    }
    result.push_back(node->symbol);
}

//  getYield() – Get all terminal symbols in left-to-right order

vector<string> ParseTree::getYield() const {
    vector<string> result;
    yieldHelper(root, result);
    return result;
}

void ParseTree::yieldHelper(shared_ptr<TreeNode> node,
                            vector<string>& result) const {
    if (!node) return;
    
    if (node->isLeaf()) {
        result.push_back(node->symbol);
    } else {
        for (auto& child : node->children) {
            yieldHelper(child, result);
        }
    }
}

//  getStats() – Calculate tree statistics

ParseTree::TreeStats ParseTree::getStats() const {
    TreeStats stats;
    if (root) {
        statsHelper(root, stats, 0);
    }
    return stats;
}

void ParseTree::statsHelper(shared_ptr<TreeNode> node,
                            TreeStats& stats,
                            int depth) const {
    if (!node) return;
    
    stats.totalNodes++;
    
    if (node->isLeaf()) {
        stats.terminalNodes++;
    } else {
        stats.nonTerminalNodes++;
    }
    
    stats.height = max(stats.height, depth + 1);
    
    for (auto& child : node->children) {
        statsHelper(child, stats, depth + 1);
    }
}

