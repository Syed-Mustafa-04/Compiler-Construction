%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

extern int yylex();
extern int line_num;
extern int col_num;
void yyerror(const char *s);

ASTNode *root;
%}

%union {
    char    *str;
    ASTNode *node;
}

%token LBRACE RBRACE LBRACKET RBRACKET COLON COMMA NUL
%token <str>  STRING NUMBER BOOL_VAL
%type  <node> json value object array members member items

%%

json:
    value               { root = $1; }
;

value:
    object              { $$ = $1; }
  | array               { $$ = $1; }
  | STRING              {
                            $$ = create_node(NODE_STRING);
                            $$->str_val = $1;
                        }
  | NUMBER              {
                            $$ = create_node(NODE_NUMBER);
                            $$->str_val = $1;
                        }
  | BOOL_VAL            {
                            $$ = create_node(NODE_BOOL);
                            $$->str_val = $1;
                        }
  | NUL                 { $$ = create_node(NODE_NULL); }
;

object:
    LBRACE members RBRACE   { $$ = $2; }
  | LBRACE RBRACE           { $$ = create_node(NODE_OBJECT); }
;

members:
    member                  {
                                $$ = create_node(NODE_OBJECT);
                                add_child($$, $1);
                            }
  | members COMMA member    {
                                $$ = $1;
                                add_child($$, $3);
                            }
;

member:
    STRING COLON value      {
                                $$ = create_node(NODE_PAIR);
                                $$->key = $1;
                                add_child($$, $3);
                            }
;

array:
    LBRACKET items RBRACKET { $$ = $2; }
  | LBRACKET RBRACKET       { $$ = create_node(NODE_ARRAY); }
;

items:
    value                   {
                                $$ = create_node(NODE_ARRAY);
                                add_child($$, $1);
                            }
  | items COMMA value       {
                                $$ = $1;
                                add_child($$, $3);
                            }
;

%%

void yyerror(const char *s) {
    fprintf(stderr,
        "Syntax Error: unexpected token at line %d, col %d\n",
        line_num, col_num);
    exit(1);
}