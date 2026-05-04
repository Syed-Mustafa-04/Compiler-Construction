#ifndef YY_YY_PARSER_TAB_H_INCLUDED
# define YY_YY_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    LBRACE = 258,                  /* LBRACE  */
    RBRACE = 259,                  /* RBRACE  */
    LBRACKET = 260,                /* LBRACKET  */
    RBRACKET = 261,                /* RBRACKET  */
    COLON = 262,                   /* COLON  */
    COMMA = 263,                   /* COMMA  */
    NUL = 264,                     /* NUL  */
    STRING = 265,                  /* STRING  */
    NUMBER = 266,                  /* NUMBER  */
    BOOL_VAL = 267                 /* BOOL_VAL  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 15 "parser.y"

    char    *str;
    ASTNode *node;

#line 81 "parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
