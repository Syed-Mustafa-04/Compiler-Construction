// ============================================================
//  tree.h  –  Parse Tree Generation for LL(1) Parser
//  CS4031 Compiler Construction  –  Assignment 02 (Task 2.5)
//
//  Parse Tree Requirements:
//    • Generate parse tree for successfully parsed strings
//    • Tree structure shows derivation steps
//    • Display in readable format (text-based ASCII art)
//    • Root: Start symbol
//    • Internal nodes: Non-terminals
//    • Leaves: Terminals
//
//  Display Options:
//    • ASCII art tree format
//    • Preorder traversal
//    • Postorder traversal
//    • Indented text format
// ============================================================

#ifndef TREE_H
#define TREE_H

#include <string>
#include <vector>
#include <memory>

using namespace std;

// ============================================================
//  TreeNode – Single node in the parse tree
// ============================================================
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

// ============================================================
//  ParseTree – Complete parse tree structure and operations
// ============================================================
class ParseTree {
public:
    // ── Construction ─────────────────────────────────────────
    ParseTree();
    ~ParseTree();
    
    // Disable copy semantics (using shared_ptr for automatic cleanup)
    ParseTree(const ParseTree&) = delete;
    ParseTree& operator=(const ParseTree&) = delete;
    
    // ── Tree building ────────────────────────────────────────
    // Get the root node (usually the start symbol)
    shared_ptr<TreeNode> getRoot() const { return root; }
    
    // Set the root node
    void setRoot(shared_ptr<TreeNode> newRoot) { root = newRoot; }
    
    // Create a new tree node
    static shared_ptr<TreeNode> createNode(const string& symbol) {
        return make_shared<TreeNode>(symbol);
    }
    
    // ── Display methods ──────────────────────────────────────
    /**
     * Display the tree to stdout in ASCII art format
     * Shows hierarchical structure with connecting lines
     */
    void display() const;
    
    /**
     * Write the tree to a file in multiple formats
     */
    void writeToFile(const string& filename) const;
    
    /**
     * Display tree in indented format (simpler representation)
     */
    void displayIndented() const;
    
    /**
     * Display tree in indented format to file
     */
    void writeIndentedToFile(const string& filename, bool append = true) const;
    
    /**
     * Preorder traversal: Root, Left, Right
     * Returns list of symbols visited in preorder
     */
    vector<string> preorderTraversal() const;
    
    /**
     * Postorder traversal: Left, Right, Root
     * Returns list of symbols visited in postorder
     */
    vector<string> postorderTraversal() const;
    
    /**
     * Get the yield (leaf sequence) of the tree
     * Returns all terminal symbols in left-to-right order
     */
    vector<string> getYield() const;
    
    /**
     * Get tree statistics (height, node count, etc.)
     */
    struct TreeStats {
        int totalNodes = 0;
        int terminalNodes = 0;
        int nonTerminalNodes = 0;
        int height = 0;
    };
    TreeStats getStats() const;
    
    /**
     * Clear the tree (delete all nodes)
     */
    void clear();
    
    /**
     * Check if tree is empty
     */
    bool isEmpty() const { return root == nullptr; }

private:
    shared_ptr<TreeNode> root;
    
    // ── Display helpers ──────────────────────────────────────
    void displayHelper(shared_ptr<TreeNode> node, 
                       const string& prefix, 
                       bool isLast,
                       ostream& out) const;
    
    void displayIndentedHelper(shared_ptr<TreeNode> node,
                               int depth,
                               ostream& out) const;
    
    // ── Traversal helpers ────────────────────────────────────
    void preorderHelper(shared_ptr<TreeNode> node,
                        vector<string>& result) const;
    
    void postorderHelper(shared_ptr<TreeNode> node,
                         vector<string>& result) const;
    
    void yieldHelper(shared_ptr<TreeNode> node,
                     vector<string>& result) const;
    
    // ── Statistics helpers ───────────────────────────────────
    void statsHelper(shared_ptr<TreeNode> node,
                     TreeStats& stats,
                     int depth) const;
};

#endif // TREE_H
