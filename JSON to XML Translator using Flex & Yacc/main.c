#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

extern int    yyparse();
extern ASTNode *root;

/* ── XML character escaping ── */
static void print_escaped(const char *s) {
    for (; *s; s++) {
        switch (*s) {
            case '&':  printf("&amp;");  break;
            case '<':  printf("&lt;");   break;
            case '>':  printf("&gt;");   break;
            case '"':  printf("&quot;"); break;
            default:   putchar(*s);      break;
        }
    }
}

/* ── Indentation helper (bonus: pretty print) ── */
static void indent(int depth) {
    for (int i = 0; i < depth; i++) printf("  ");
}

/* ── Recursive XML generator ── */
static void generate_xml(ASTNode *node, int depth) {
    if (!node) return;

    switch (node->type) {

        case NODE_OBJECT:
            for (int i = 0; i < node->child_count; i++)
                generate_xml(node->children[i], depth);
            break;

        case NODE_PAIR: {
            char    *tag = node->key;
            ASTNode *val = node->children[0];

            indent(depth);

            if (val->type == NODE_NULL) {
                printf("<%s/>\n", tag);

            } else if (val->type == NODE_OBJECT) {
                printf("<%s>\n", tag);
                generate_xml(val, depth + 1);
                indent(depth);
                printf("</%s>\n", tag);

            } else if (val->type == NODE_ARRAY) {
                printf("<%s>\n", tag);
                generate_xml(val, depth + 1);
                indent(depth);
                printf("</%s>\n", tag);

            } else {
                /* scalar: STRING, NUMBER, BOOL */
                printf("<%s>", tag);
                print_escaped(val->str_val);
                printf("</%s>\n", tag);
            }
            break;
        }

        case NODE_ARRAY:
            for (int i = 0; i < node->child_count; i++) {
                ASTNode *item = node->children[i];
                indent(depth);
                if (item->type == NODE_NULL) {
                    printf("<item/>\n");
                } else if (item->type == NODE_OBJECT ||
                           item->type == NODE_ARRAY) {
                    printf("<item>\n");
                    generate_xml(item, depth + 1);
                    indent(depth);
                    printf("</item>\n");
                } else {
                    printf("<item>");
                    print_escaped(item->str_val);
                    printf("</item>\n");
                }
            }
            break;

        default:
            break;
    }
}

int main(int argc, char *argv[]) {
    /* parse stdin → builds AST */
    yyparse();

    /* write AST to a file for post-run inspection */
    FILE *ast_file = fopen("ast.txt", "w");
    if (ast_file) {
        print_ast(root, 0, ast_file);
        fclose(ast_file);
    } else {
        fprintf(stderr, "Failed to open ast.txt for writing\n");
    }

    /* generate XML */
    printf("<root>\n");
    generate_xml(root, 1);
    printf("</root>\n");

    free_ast(root);
    return 0;
}