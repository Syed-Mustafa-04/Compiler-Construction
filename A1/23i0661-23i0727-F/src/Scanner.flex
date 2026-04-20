import java.io.*;

%%

%public
%class Yylex
%unicode
%line
%column
%type Token

%{

    private SymbolTable symbolTable;
    private ErrorHandler errorHandler;

    /* Comment counter */
    private int commentCount = 0;

    public int getCommentCount() {
        return commentCount;
    }

public int getLineCount() {
    return yyline + 1;
}

    public void setSymbolTable(SymbolTable table) {
        this.symbolTable = table;
    }

    public void setErrorHandler(ErrorHandler handler) {
        this.errorHandler = handler;
    }

    private Token createToken(TokenType type) {

        String lexeme = yytext();

        return new Token(
            type,
            lexeme,
            yyline + 1,
            yycolumn + 1
        );
    }

    private Token createErrorToken(
            ErrorHandler.ErrorType type,
            String reason) {

        if (errorHandler != null) {

            errorHandler.reportError(
                type,
                yyline + 1,
                yycolumn + 1,
                yytext(),
                reason
            );
        }

        return new Token(
            TokenType.ERROR,
            yytext(),
            yyline + 1,
            yycolumn + 1
        );
    }
%}

/* ---------- MACROS ---------- */

DIGIT   = [0-9]
LOWER   = [a-z]
UPPER   = [A-Z]

IDBODY  = ({LOWER}|{DIGIT}|_)
DIGITS  = {DIGIT}+

IDENTIFIER_VALID = {UPPER}({IDBODY}){0,30}
IDENTIFIER_TOO_LONG = {UPPER}({IDBODY}){31}{IDBODY}*
IDENTIFIER_INVALID = {UPPER}({IDBODY}*{UPPER}+{IDBODY}*)+

INTEGER    = [+-]?{DIGITS}
FLOAT      = [+-]?{DIGITS}\.{DIGIT}{1,6}([eE][+-]?{DIGITS})?
BOOLEAN    = (true|false)

STRING     = \"([^\"\\\n]|\\[\"\\ntr])*\" 
COMMENT    = ##[^\n]* 
WHITESPACE = [ \t\r\n]+

%%

/* ---------- ERROR RULES ---------- */

{IDENTIFIER_TOO_LONG} {
    return createErrorToken(
        ErrorHandler.ErrorType.IDENTIFIER_TOO_LONG,
        "Identifier too long"
    );
}

{IDENTIFIER_INVALID} {
    return createErrorToken(
        ErrorHandler.ErrorType.INVALID_IDENTIFIER,
        "Invalid identifier"
    );
}

[+-]?{DIGITS}[A-Za-z_][A-Za-z0-9_]* {
    return createErrorToken(
        ErrorHandler.ErrorType.MALFORMED_INTEGER,
        "Malformed integer"
    );
}

[+-]?{DIGITS}\.{DIGIT}{6}{DIGIT}+ {
    return createErrorToken(
        ErrorHandler.ErrorType.MALFORMED_FLOAT,
        "Too many decimals"
    );
}

\"([^\"\\\n]|\\.)* {
    return createErrorToken(
        ErrorHandler.ErrorType.MALFORMED_STRING,
        "Unterminated string"
    );
}

/* ---------- NORMAL RULES ---------- */

/* Comment (SKIP + COUNT) */
{COMMENT} {
    commentCount++;
}

/* Whitespace (SKIP) */
{WHITESPACE} {
    /* skip */
}

/* Boolean */
{BOOLEAN} {
    return createToken(TokenType.BOOLEAN_LITERAL);
}

/* Float */
{FLOAT} {
    return createToken(TokenType.FLOAT_LITERAL);
}

/* Integer */
{INTEGER} {
    return createToken(TokenType.INTEGER_LITERAL);
}

/* Identifier */
{IDENTIFIER_VALID} {
    return createToken(TokenType.IDENTIFIER);
}

/* String */
{STRING} {
    return createToken(TokenType.STRING_LITERAL);
}

/* Invalid character */
. {
    return createErrorToken(
        ErrorHandler.ErrorType.INVALID_CHARACTER,
        "Invalid character"
    );
}

/* EOF */
<<EOF>> {

    return new Token(
        TokenType.EOF,
        "EOF",
        yyline + 1,
        yycolumn + 1
    );
}
