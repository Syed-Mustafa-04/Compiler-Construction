#include "stack.h"
#include <iostream>
#include <algorithm>

using namespace std;

Stack::Stack() : topNode_(nullptr), size_(0) {}

Stack::~Stack() {
    clear();
}

void Stack::push(const string& symbol) {
    topNode_ = new StackNode(symbol, topNode_);
    size_++;
}

string Stack::pop() {
    if (isEmpty()) {
        throw underflow_error("Stack::pop() stack is empty");
    }
    StackNode* temp = topNode_;
    string symbol = temp->symbol;
    topNode_ = temp->next;
    delete temp;
    size_--;
    return symbol;
}


const string& Stack::top() const {
    if (isEmpty()) {
        throw underflow_error("Stack::top() stack is empty");
    }
    return topNode_->symbol;
}

bool Stack::isEmpty() const {
    return topNode_ == nullptr;
}

int Stack::size() const {
    return size_;
}


void Stack::clear() {
    while (!isEmpty()) {
        pop();
    }
}


void Stack::initForParsing(const string& startSymbol) {
    clear();
    push("$");
    push(startSymbol);
}


void Stack::pushProduction(const vector<string>& symbols) {
    // Push in reverse order so the first element ends up on top
    for (int i = static_cast<int>(symbols.size()) - 1; i >= 0; --i) {
        push(symbols[i]);
    }
}


string Stack::toString() const {
    if (isEmpty()) {
        return "";
    }

    // Collect all symbols from bottom to top
    vector<string> symbols = toVector();

    // Build string
    string result;
    for (size_t i = 0; i < symbols.size(); ++i) {
        if (i > 0) result += " ";
        result += symbols[i];
    }
    return result;
}


void Stack::print() const {
    cout << toString() << endl;
}


vector<string> Stack::toVector() const {
    vector<string> result;

    // Traverse from top to bottom
    StackNode* current = topNode_;
    while (current != nullptr) {
        result.push_back(current->symbol);
        current = current->next;
    }

    // Reverse to get bottom-to-top order
    reverse(result.begin(), result.end());

    return result;
}


void Stack::pushWithTreeNode(const string& symbol, shared_ptr<TreeNode> treeNode) {
    topNode_ = new StackNode(symbol, topNode_, treeNode);
    size_++;
}

void Stack::pushProductionWithTreeNodes(const vector<string>& symbols,
                                        const vector<shared_ptr<TreeNode>>& treeNodes) {
    // Push in reverse order so the first element ends up on top
    // symbols[0] should be on top after this operation
    for (int i = static_cast<int>(symbols.size()) - 1; i >= 0; --i) {
        shared_ptr<TreeNode> node = nullptr;
        if (i < (int)treeNodes.size()) {
            node = treeNodes[i];
        }
        pushWithTreeNode(symbols[i], node);
    }
}


shared_ptr<TreeNode> Stack::getTopTreeNode() const {
    if (isEmpty()) {
        return nullptr;
    }
    return topNode_->treeNode;
}

shared_ptr<TreeNode> Stack::popTreeNode() {
    if (isEmpty()) {
        return nullptr;
    }
    shared_ptr<TreeNode> node = topNode_->treeNode;
    // Does NOT call pop() caller must handle stack pop separately
    return node;
}
