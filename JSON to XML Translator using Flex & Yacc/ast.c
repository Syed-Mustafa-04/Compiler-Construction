#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

ASTNode *create_node(NodeType type) {
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type        = type;
    node->key         = NULL;
    node->str_val     = NULL;
    node->children    = NULL;
    node->child_count = 0;
    return node;
}

void add_child(ASTNode *parent, ASTNode *child) {
    parent->children = realloc(parent->children,
                       sizeof(ASTNode *) * (parent->child_count + 1));
    parent->children[parent->child_count++] = child;
}

void free_ast(ASTNode *node) {
    if (!node) return;
    free(node->key);
    free(node->str_val);
    for (int i = 0; i < node->child_count; i++)
        free_ast(node->children[i]);
    free(node->children);
    free(node);
}

/* Bonus: print AST structure */
static const char *node_type_name(NodeType t) {
    switch (t) {
        case NODE_OBJECT: return "OBJECT";
        case NODE_ARRAY:  return "ARRAY";
        case NODE_PAIR:   return "PAIR";
        case NODE_STRING: return "STRING";
        case NODE_NUMBER: return "NUMBER";
        case NODE_BOOL:   return "BOOL";
        case NODE_NULL:   return "NULL";
        default:          return "UNKNOWN";
    }
}

void print_ast(ASTNode *node, int depth) {
    if (!node) return;
    for (int i = 0; i < depth; i++) printf("  ");
    printf("[%s]", node_type_name(node->type));
    if (node->key)     printf(" key=\"%s\"",     node->key);
    if (node->str_val) printf(" value=\"%s\"", node->str_val);
    printf("\n");
    for (int i = 0; i < node->child_count; i++)
        print_ast(node->children[i], depth + 1);
}