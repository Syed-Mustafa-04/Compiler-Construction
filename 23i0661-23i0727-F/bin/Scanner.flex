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

    public void setSymbolTable(SymbolTable table) 
    {
        this.symbolTable = table;
    }

    public void setErrorHandler(ErrorHandler handler) {
        this.errorHandler = handler;
    }

    private Token createToken(TokenType type) {

        String lexeme = yytext();

        if (type == TokenType.IDENTIFIER && symbolTable != null) {
            symbolTable.addIdentifier(
                lexeme,
                type,
                yyline + 1,
                yycolumn + 1
            );
        }

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

/* ======================
   MACROS
   ====================== */

DIGIT   = [0-9]
LOWER   = [a-z]
UPPER   = [A-Z]

IDBODY  = ({LOWER}|{DIGIT}|_)
DIGITS  = {DIGIT}+  

/* Valid identifier: starts with uppercase, followed by lowercase/digits/_, max 31 chars */
IDENTIFIER_VALID   = {UPPER}{IDBODY}{0,30}

/* Invalid identifier: starts with uppercase, contains uppercase letters after first char, max 31 chars */
IDENTIFIER_INVALID = {UPPER}({IDBODY}*{UPPER}+{IDBODY}*){0,30}

INTEGER    = [+-]?{DIGITS}
FLOAT      = [+-]?{DIGITS}\.{DIGIT}{1,6}([eE][+-]?{DIGITS})?
BOOLEAN    = (true|false)
STRING     = \"([^\"\\\n]|\\[\"\\ntr])*\" 
COMMENT    = ##[^\n]* 
WHITESPACE = [ \t\r\n]+

%%

/* ======================
   ERROR RULES (FIRST)
   ====================== */

/* Identifier too long (more than 31 chars) */
{UPPER}{IDBODY}{31}{IDBODY}+ {
    return createErrorToken(
        ErrorHandler.ErrorType.IDENTIFIER_TOO_LONG,
        "Identifier exceeds maximum length"
    );
}

/* Invalid identifier (uppercase inside) - must come before valid identifier */
{IDENTIFIER_INVALID} {
    return createErrorToken(
        ErrorHandler.ErrorType.INVALID_IDENTIFIER,
        "Identifier contains uppercase letters after the first character"
    );
}

/* Malformed integer (letters in number) */
[+-]?{DIGITS}[A-Za-z_][A-Za-z0-9_]* {
    return createErrorToken(
        ErrorHandler.ErrorType.MALFORMED_INTEGER,
        "Malformed integer literal"
    );
}

/* Malformed float (too many decimals) */
[+-]?{DIGITS}\.{DIGIT}{6}{DIGIT}+ {
    return createErrorToken(
        ErrorHandler.ErrorType.MALFORMED_FLOAT,
        "Too many decimal places"
    );
}

/* Unterminated string */
\"([^\"\\\n]|\\.)* {
    return createErrorToken(
        ErrorHandler.ErrorType.MALFORMED_STRING,
        "Unterminated string literal"
    );
}

/* ======================
   NORMAL RULES
   ====================== */

/* 1. Single-line comment */
{COMMENT} {
    return createToken(TokenType.SINGLE_LINE_COMMENT);
}

/* 2. Whitespace */
{WHITESPACE} {
    return createToken(TokenType.WHITESPACE);
}

/* 3. Boolean literal */
{BOOLEAN} {
    return createToken(TokenType.BOOLEAN_LITERAL);
}

/* 4. Floating-point literal */
{FLOAT} {
    return createToken(TokenType.FLOAT_LITERAL);
}

/* 5. Integer literal */
{INTEGER} {
    return createToken(TokenType.INTEGER_LITERAL);
}

/* 6. Valid identifier */
{IDENTIFIER_VALID} {
    return createToken(TokenType.IDENTIFIER);
}

/* 7. String literal */
{STRING} {
    return createToken(TokenType.STRING_LITERAL);
}

/* ======================
   DEFAULT ERROR
   ====================== */

. {
    return createErrorToken(
        ErrorHandler.ErrorType.INVALID_CHARACTER,
        "Invalid character"
    );
}

/* ======================
   EOF
   ====================== */

<<EOF>> 
{
    return new Token(
        TokenType.EOF,
        "EOF",
        yyline + 1,
        yycolumn + 1
    );
}
