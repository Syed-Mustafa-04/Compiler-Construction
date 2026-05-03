%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "parser.tab.h"

int line_num = 1;
int col_num  = 1;

static int hex_val(int c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static int utf8_encode(unsigned int codepoint, char *out) {
    if (codepoint <= 0x7F) {
        out[0] = (char)codepoint;
        return 1;
    }
    if (codepoint <= 0x7FF) {
        out[0] = (char)(0xC0 | (codepoint >> 6));
        out[1] = (char)(0x80 | (codepoint & 0x3F));
        return 2;
    }
    if (codepoint <= 0xFFFF) {
        out[0] = (char)(0xE0 | (codepoint >> 12));
        out[1] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
        out[2] = (char)(0x80 | (codepoint & 0x3F));
        return 3;
    }
    if (codepoint <= 0x10FFFF) {
        out[0] = (char)(0xF0 | (codepoint >> 18));
        out[1] = (char)(0x80 | ((codepoint >> 12) & 0x3F));
        out[2] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
        out[3] = (char)(0x80 | (codepoint & 0x3F));
        return 4;
    }
    return 0;
}

static char *decode_json_string(const char *text) {
    size_t len = strlen(text);
    size_t alloc = len * 4 + 1;
    char *out = malloc(alloc);
    if (!out) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    const char *p = text + 1;
    const char *end = text + len - 1;
    char *q = out;

    while (p < end) {
        if (*p == '\\') {
            p++;
            if (p >= end) {
                fprintf(stderr,
                    "Lexical Error: invalid escape sequence at line %d, col %d\n",
                    line_num, col_num);
                exit(1);
            }
            switch (*p) {
                case '"': *q++ = '"'; break;
                case '\\': *q++ = '\\'; break;
                case '/': *q++ = '/'; break;
                case 'b': *q++ = '\b'; break;
                case 'f': *q++ = '\f'; break;
                case 'n': *q++ = '\n'; break;
                case 'r': *q++ = '\r'; break;
                case 't': *q++ = '\t'; break;
                case 'u': {
                    if (p + 4 >= end) {
                        fprintf(stderr,
                            "Lexical Error: invalid unicode escape at line %d, col %d\n",
                            line_num, col_num);
                        exit(1);
                    }
                    unsigned int code = 0;
                    for (int i = 1; i <= 4; i++) {
                        int v = hex_val(*(p + i));
                        if (v < 0) {
                            fprintf(stderr,
                                "Lexical Error: invalid unicode escape at line %d, col %d\n",
                                line_num, col_num);
                            exit(1);
                        }
                        code = (code << 4) | v;
                    }
                    p += 4;

                    if (code >= 0xD800 && code <= 0xDBFF) {
                        if (p + 6 >= end || *(p + 1) != '\\' || *(p + 2) != 'u') {
                            fprintf(stderr,
                                "Lexical Error: invalid unicode surrogate at line %d, col %d\n",
                                line_num, col_num);
                            exit(1);
                        }
                        unsigned int low = 0;
                        for (int i = 3; i <= 6; i++) {
                            int v = hex_val(*(p + i));
                            if (v < 0) {
                                fprintf(stderr,
                                    "Lexical Error: invalid unicode escape at line %d, col %d\n",
                                    line_num, col_num);
                                exit(1);
                            }
                            low = (low << 4) | v;
                        }
                        if (low < 0xDC00 || low > 0xDFFF) {
                            fprintf(stderr,
                                "Lexical Error: invalid unicode surrogate pair at line %d, col %d\n",
                                line_num, col_num);
                            exit(1);
                        }
                        code = 0x10000 + (((code - 0xD800) << 10) | (low - 0xDC00));
                        p += 6;
                    } else if (code >= 0xDC00 && code <= 0xDFFF) {
                        fprintf(stderr,
                            "Lexical Error: invalid unicode escape at line %d, col %d\n",
                            line_num, col_num);
                        exit(1);
                    }

                    int used = utf8_encode(code, q);
                    if (used == 0) {
                        fprintf(stderr,
                            "Lexical Error: invalid unicode codepoint at line %d, col %d\n",
                            line_num, col_num);
                        exit(1);
                    }
                    q += used;
                    break;
                }

                default:
                    fprintf(stderr,
                        "Lexical Error: invalid escape sequence \\%c at line %d, col %d\n",
                        *p, line_num, col_num);
                    exit(1);
            }
        } else {
            *q++ = *p;
        }
        p++;
    }

    *q = '\0';
    return out;
}
%}

%option noyywrap

%%

[ \t\r]+            { col_num += yyleng; }
\n                  { line_num++; col_num = 1; }

"{"                 { col_num++; return LBRACE;   }
"}"                 { col_num++; return RBRACE;   }
"["                 { col_num++; return LBRACKET; }
"]"                 { col_num++; return RBRACKET; }
":"                 { col_num++; return COLON;    }
","                 { col_num++; return COMMA;    }

"true"              {
                        col_num += yyleng;
                        yylval.str = strdup("true");
                        return BOOL_VAL;
                    }
"false"             {
                        col_num += yyleng;
                        yylval.str = strdup("false");
                        return BOOL_VAL;
                    }
"null"              { col_num += yyleng; return NUL; }

\"([^\"\\]|\\([\"\\/bfnrt]|u[0-9a-fA-F]{4}))*\" {
                        col_num += yyleng;
                        yylval.str = decode_json_string(yytext);
                        return STRING;
                    }

\"[^\"\n]*          {
                        fprintf(stderr,
                            "Lexical Error: unterminated string at line %d, col %d\n",
                            line_num, col_num);
                        exit(1);
                    }

-?[0-9]+(\.[0-9]+)?([eE][+-]?[0-9]+)? {
                        col_num += yyleng;
                        yylval.str = strdup(yytext);
                        return NUMBER;
                    }

.                   {
                        fprintf(stderr,
                            "Lexical Error: invalid character '%s' at line %d, col %d\n",
                            yytext, line_num, col_num);
                        exit(1);
                    }

%%