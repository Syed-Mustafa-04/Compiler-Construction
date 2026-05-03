#ifndef AST_H
#define AST_H

typedef enum {
    NODE_OBJECT,
    NODE_ARRAY,
    NODE_PAIR,
    NODE_STRING,
    NODE_NUMBER,
    NODE_BOOL,
    NODE_NULL
} NodeType;

typedef struct ASTNode {
    NodeType type;
    char *key;
    char *str_val;
    struct ASTNode **children;
    int child_count;
} ASTNode;

ASTNode *create_node(NodeType type);
void add_child(ASTNode *parent, ASTNode *child);
void free_ast(ASTNode *node);
void print_ast(ASTNode *node, int depth);  /* bonus: AST printing */

#endif