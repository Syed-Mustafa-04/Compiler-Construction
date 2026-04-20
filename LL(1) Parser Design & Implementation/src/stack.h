#ifndef STACK_H
#define STACK_H

#include <string>
#include <vector>
#include <stdexcept>
#include <memory>

using namespace std;

// Forward declaration
struct TreeNode;

struct StackNode {
    string symbol;                      // grammar symbol stored at this node
    shared_ptr<TreeNode> treeNode;      // associated parse tree node (optional)
    StackNode* next;                         // pointer toward the bottom of the stack

    explicit StackNode(const string& sym, StackNode* nxt = nullptr,
                       shared_ptr<TreeNode> tree = nullptr)
        : symbol(sym), treeNode(tree), next(nxt) {}
};

class Stack {
public:
  
    Stack();
    ~Stack();

    // Disable copy to avoid double-free on raw pointers
    Stack(const Stack&)            = delete;
    Stack& operator=(const Stack&) = delete;

    // core operations 

    /**
  
      @param symbol  Any terminal, non-terminal, or "$".
     */
    void push(const string& symbol);

    void pushProduction(const vector<string>& symbols);

    /**
     * Remove and return the top symbol.
     * @throws underflow_error if the stack is empty.
     */
    string pop();

    /**
     * Return (but do NOT remove) the top symbol.
     * @throws underflow_error if the stack is empty.
     */
    const string& top() const;

    bool isEmpty() const;

    int size() const;

    void clear();

    /**

     *
     * @param startSymbol  The start non-terminal of the grammar.
     */
    void initForParsing(const string& startSymbol);

    // ---- display helpers

    string toString() const;

    /**
     * Print the stack to stdout (bottom→top on one line).
     */
    void print() const;

 
    vector<string> toVector() const;

    /**
   
     * @param symbol    The grammar symbol.
     * @param treeNode  The associated parse tree node (optional).
     */
    void pushWithTreeNode(const string& symbol, shared_ptr<TreeNode> treeNode);

    /**
  
     * @param symbols   The production symbols (left-to-right order).
     * @param treeNodes The corresponding tree nodes (same order as symbols).
     */
    void pushProductionWithTreeNodes(const vector<string>& symbols,
                                     const vector<shared_ptr<TreeNode>>& treeNodes);

    /**
    
     * @return The tree node, or nullptr if none is associated.
     */
    shared_ptr<TreeNode> getTopTreeNode() const;

    /**

     * @return The tree node, or nullptr if none is associated.
     */
    shared_ptr<TreeNode> popTreeNode();

private:
    StackNode* topNode_;   // pointer to the top (head) of the linked list
    int        size_;      // cached element count for O(1) size()
};

#endif 
