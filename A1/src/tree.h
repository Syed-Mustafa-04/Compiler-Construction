#ifndef TREE_H
#define TREE_H

#include <string>
#include <vector>
#include <memory>

using namespace std;

//  TreeNode – Single node in the parse tree
struct TreeNode {
    string symbol;                    // Non-terminal or terminal symbol
    vector<shared_ptr<TreeNode>> children;  // Child nodes
    
    explicit TreeNode(const string& sym) : symbol(sym) {}
    
    // Check if this is a leaf node (terminal)
    bool isLeaf() const { return children.empty(); }
    
    // Add a child node
    void addChild(shared_ptr<TreeNode> child) {
        children.push_back(child);
    }
};

//  ParseTree – Complete parse tree structure and operations
class ParseTree {
public:
    ParseTree();
    ~ParseTree();
    
    // Disable copy semantics (using shared_ptr for automatic cleanup)
    ParseTree(const ParseTree&) = delete;
    ParseTree& operator=(const ParseTree&) = delete;
    
    // ── Tree building 
    // Get the root node (usually the start symbol)
    shared_ptr<TreeNode> getRoot() const { return root; }
    
    // Set the root node
    void setRoot(shared_ptr<TreeNode> newRoot) { root = newRoot; }
    
    // Create a new tree node
    static shared_ptr<TreeNode> createNode(const string& symbol) {
        return make_shared<TreeNode>(symbol);
    }
    
    // ── Display methods 
    /**
     * Display the tree to stdout in ASCII art format
     * Shows hierarchical structure with connecting lines
     */
    void display() const;
    
    void writeToFile(const string& filename) const;
    
    void displayIndented() const;
    
    void writeIndentedToFile(const string& filename, bool append = true) const;
    
    vector<string> preorderTraversal() const;
    
    vector<string> postorderTraversal() const;
    
    vector<string> getYield() const;
    
    struct TreeStats {
        int totalNodes = 0;
        int terminalNodes = 0;
        int nonTerminalNodes = 0;
        int height = 0;
    };
    TreeStats getStats() const;
    
    void clear();
    
    bool isEmpty() const { return root == nullptr; }

private:
    shared_ptr<TreeNode> root;
    
    void displayHelper(shared_ptr<TreeNode> node, 
                       const string& prefix, 
                       bool isLast,
                       ostream& out) const;
    
    void displayIndentedHelper(shared_ptr<TreeNode> node,
                               int depth,
                               ostream& out) const;
    
    // ── Traversal helpers
    void preorderHelper(shared_ptr<TreeNode> node,
                        vector<string>& result) const;
    
    void postorderHelper(shared_ptr<TreeNode> node,
                         vector<string>& result) const;
    
    void yieldHelper(shared_ptr<TreeNode> node,
                     vector<string>& result) const;
    
    // ── Statistics helpers
    void statsHelper(shared_ptr<TreeNode> node,
                     TreeStats& stats,
                     int depth) const;
};

#endif 
