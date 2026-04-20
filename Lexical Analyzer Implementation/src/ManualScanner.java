import java.util.*;
import java.io.*;

public class ManualScanner {

    private String input;
    private int position;
    private int line;
    private int column;

    private List<Token> tokens = new ArrayList<>();
    private Map<TokenType, Integer> statistics = new HashMap<>();
    
    private int whitespacesSkipped = 0;
    private int commentsSkipped = 0;

    private SymbolTable symbolTable = new SymbolTable();
    private ErrorHandler errorHandler = new ErrorHandler();

    public ManualScanner(String input) {
        this.input = input;
        this.position = 0;
        this.line = 1;
        this.column = 1;

        for (TokenType type : TokenType.values()) {
            statistics.put(type, 0);
        }
    }

    public List<Token> scan() {

        while (position < input.length()) {

            char current = peek();

            // Skip newline/tab/carriage-return characters which are separators in the DFA
            if (current == '\n' || current == '\r' || current == '\t') {
                advance();
                continue;
            }

            // Whitespace (spaces) are still handled by the DFA
            Token token = nextToken();

            if (token != null) {
                // Don't count comments in the total token count
                if (!token.getType().toString().equals("SINGLE_LINE_COMMENT")) {
                    tokens.add(token);
                    statistics.put(token.getType(),
                            statistics.get(token.getType()) + 1);
                }
            }
        }

        return tokens;
    }

    private Token nextToken() {

        int startPos = position;
        int startLine = line;
        int startColumn = column;

        // Skip spaces (whitespace) but not comments
        while (position < input.length() && peek() == ' ') {
            advance();
            whitespacesSkipped++;
        }

        // If we're at end of file, return null
        if (position >= input.length()) {
            return null;
        }

        // Check if this is a comment
        if (peek() == '#') {
            int commentStartCol = column;
            StringBuilder commentContent = new StringBuilder();
            
            // Read the entire comment line
            while (position < input.length() && peek() != '\n' && peek() != '\r') {
                commentContent.append(peek());
                advance();
            }
            commentsSkipped++;
            
            // Create a comment token but don't count it in statistics
            return new Token(TokenType.SINGLE_LINE_COMMENT, commentContent.toString(), startLine, commentStartCol);
        }

        // Update start position after skipping whitespace
        startPos = position;
        startLine = line;
        startColumn = column;

        // Quick path: handle lowercase-starting keywords or error
        if (position < input.length() && isLowerCase(peek()) && peek() != 't' && peek() != 'f') {
            int kwLine = line;
            int kwCol = column;
            StringBuilder sb = new StringBuilder();
            while (position < input.length() && isLowerCase(peek())) {
                sb.append(peek());
                advance();
            }
            String kw = sb.toString();
            if (isKeyword(kw)) {
                return new Token(TokenType.KEYWORD, kw, kwLine, kwCol);
            } else {
                // Lowercase identifiers are not allowed - this is an error
                String errorMsg = "Identifier must start with uppercase letter";
                errorHandler.add(kw, errorMsg, kwLine, kwCol);
                return null;
            }
        }

        int state = 0;
        int lastAcceptState = -1;
        int lastAcceptPos = -1;

        while (position < input.length()) {

            char c = peek();
            int nextState = transition(state, c);

            if (nextState == -1)
                break;

            state = nextState;
            advance();

            if (isAccepting(state)) {
                lastAcceptState = state;
                lastAcceptPos = position;
            }
        }

        if (lastAcceptState == -1) {
            // Build the lexeme that caused the error
            String errorLexeme = input.substring(startPos, Math.min(position, input.length()));
            String errorMsg = "Mixed-case identifier not allowed";
            
            // Special error messages for string-related errors
            if (state == 5 || state == 11) {
                // We were in a string and hit an error
                if (position >= input.length()) {
                    errorMsg = "Unterminated string literal";
                } else if (state == 11) {
                    errorMsg = "Invalid escape sequence in string";
                } else if (peek() == '\n') {
                    errorMsg = "Newline not allowed in string literal";
                }
            }
            
            // Log the error
            errorHandler.add(errorLexeme, errorMsg, startLine, startColumn);
            
            if (position < input.length()) {
                advance();
            }
            return null;
        }

        String lexeme = input.substring(startPos, lastAcceptPos);
        position = lastAcceptPos;

        // Check if identifier is followed by another uppercase letter (which would form mixed-case)
        if ((lastAcceptState == 9 || lastAcceptState == 10) && position < input.length()) {
            char nextChar = peek();
            if (isUpperCase(nextChar)) {
                // Identifier followed directly by another uppercase letter is an error
                // Build the full problematic lexeme
                int errorStart = startPos;
                int errorEnd = position;
                while (errorEnd < input.length() && 
                       (isUpperCase(input.charAt(errorEnd)) || 
                        isLowerCase(input.charAt(errorEnd)) || 
                        isDigit(input.charAt(errorEnd)) || 
                        input.charAt(errorEnd) == '_')) {
                    errorEnd++;
                }
                String fullLexeme = input.substring(errorStart, errorEnd);
                String errorMsg = "Mixed-case identifier not allowed";
                errorHandler.add(fullLexeme, errorMsg, startLine, startColumn);
                position = errorEnd;
                return null;
            }
        }
        return createToken(lastAcceptState, lexeme, startLine, startColumn);
    }

    private int transition(int state, char c) {

        switch (state) {

            case 0: // Initial state (q0)
                if (isDigit(c)) return 2;              // D -> q2 (integer path)
                if (isSign(c)) return 7;               // S (+/-) -> q7
                if (c == '"') return 5;                // " -> q5 (string)
                if (isUpperCase(c)) return 9;          // U -> q9 (identifier)
                if (c == 't') return 3;                // t -> q3 (keyword path)
                if (c == 'f') return 8;                // f -> q8 (false path)
                break;

            case 2: // After digit(s) (q2) - INTEGER or FLOAT
                if (isDigit(c)) return 2;              // D -> stay q2 (more digits)
                if (c == '.') return 18;               // . -> q18 (decimal point)
                if (isExponent(c)) return 20;          // E/e -> q20 (exponent)
                break;

            case 3: // After 't' (q3)
                if (c == 'r') return 17;               // r -> q17
                break;

            case 5: // Inside string (q5)
                if (c == '"') return 16;               // closing " -> q16 (final string state)
                if (c == '\\') return 11;              // \ -> q11 (escape sequence)
                if (c == '\n') return -1;              // actual newline not allowed
                return 5;                              // any other valid char -> stay q5
                
            case 11: // After backslash in string (q11) - escape sequence
                if (c == '"' || c == '\\' || c == 'n' || c == 't' || c == 'r') {
                    return 5;                          // valid escape -> back to q5
                }
                return -1;                             // invalid escape sequence

            case 7: // After sign (q7)
                if (isDigit(c)) return 2;              // D -> q2
                break;

            case 8: // After 'f' (q8) - false keyword
                if (c == 'a') return 19;               // a -> q19
                break;

            case 9: // After uppercase (q9) - IDENTIFIER [A-Z]
                if (isLowerCase(c)) return 10;         // L -> q10 (continue with lowercase/digits/underscore)
                if (isDigit(c)) return 10;             // D -> q10 (continue with lowercase/digits/underscore)
                if (c == '_') return 10;               // _ -> q10 (continue with lowercase/digits/underscore)
                if (isUpperCase(c)) return -1;         // U -> ERROR (no more uppercase allowed)
                break;

            case 10: // After uppercase + lowercase/digit/underscore (q10) - IDENTIFIER continuation
                if (isLowerCase(c)) return 10;         // L -> stay q10
                if (isDigit(c)) return 10;             // D -> stay q10
                if (c == '_') return 10;               // _ -> stay q10
                if (isUpperCase(c)) return -1;         // U -> ERROR (uppercase not allowed after first letter)
                break;

            case 16: // After closing quote (q16) - STRING_LITERAL final
                // This is a final accepting state
                break;

            case 17: // After 'tr' (q17)
                if (c == 'u') return 1;                // u -> q1
                break;

            case 18: // After dot (q18) - decimal point
                if (isDigit(c)) return 28;             // D -> q28 (float with fractional part)
                break;

            case 19: // After 'fa' (q19)
                if (c == 'l') return 23;               // l -> q23
                break;

            case 20: // After exponent (q20)
                if (isSign(c)) return 24;              // S -> q24
                if (isDigit(c)) return 27;             // D -> q27
                break;

            case 23: // After 'fal' (q23)
                if (c == 's') return 26;               // s -> q26
                break;

            case 24: // After exponent + sign (q24)
                if (isDigit(c)) return 27;             // D -> q27
                break;

            case 26: // After 'fals' (q26)
                if (c == 'e') return 25;               // e -> q25 (false literal)
                break;

            case 27: // After exponent + digit(s) (q27) - FLOAT_LITERAL
                if (isDigit(c)) return 27;             // D -> stay q27
                break;

            case 28: // After decimal point + digit(s) (q28) - FLOAT_LITERAL
                if (isDigit(c)) return 28;             // D -> stay q28 (more fractional digits)
                if (isExponent(c)) return 20;          // E/e -> q20 (optional exponent)
                break;

            case 1: // After 'tru' (q1) - true literal
                // This is a final accepting state for "true"
                break;

            case 25: // After 'false' (q25) - BOOLEAN_LITERAL
                // This is a final accepting state
                break;
        }

        return -1;
    }

    // Helper methods for character classification
    private boolean isDigit(char c) {
        return Character.isDigit(c);
    }

    private boolean isSign(char c) {
        return c == '+' || c == '-';
    }

    private boolean isUpperCase(char c) {
        return Character.isUpperCase(c);
    }

    private boolean isLowerCase(char c) {
        return Character.isLowerCase(c);
    }

    private boolean isExponent(char c) {
        return c == 'e' || c == 'E';
    }

    private boolean isAccepting(int state) {
        // Final states from the DFA (excluding whitespace and comment states):
        // state 1: "true" (BOOLEAN_LITERAL)
        // state 2: INTEGER_LITERAL (D or D+)
        // state 9: IDENTIFIER (single uppercase letter only)
        // state 10: IDENTIFIER (uppercase + lowercase/digit/underscore continuation)
        // state 16: STRING_LITERAL (closing quote)
        // state 25: "false" (BOOLEAN_LITERAL)
        // state 27: FLOAT_LITERAL (D E D or D E S D)
        // state 28: FLOAT_LITERAL (D . D+ [E [S] D+])
        return state == 1 || state == 2 
                || state == 9 || state == 10 || state == 16 
                || state == 25 || state == 27 || state == 28;
    }

    private Token createToken(int state, String lexeme,
                              int line, int column) {

        switch (state) {

            case 1: // "true" (BOOLEAN_LITERAL)
                return new Token(TokenType.BOOLEAN_LITERAL, lexeme, line, column);

            case 2: // INTEGER_LITERAL
                // Check for malformed integer (digit followed by letter like "12abc")
                if (position < input.length() && isLowerCase(input.charAt(position))) {
                    int errorEnd = position;
                    while (errorEnd < input.length() && 
                           (isLowerCase(input.charAt(errorEnd)) || 
                            isDigit(input.charAt(errorEnd)))) {
                        errorEnd++;
                    }
                    String malformedLexeme = input.substring(position - lexeme.length(), errorEnd);
                    String errorMsg = "Malformed integer: digit followed by letter";
                    errorHandler.add(malformedLexeme, errorMsg, line, column);
                    // Advance position to skip the malformed part
                    while (position < errorEnd) {
                        advance();
                    }
                    return null;
                }
                return new Token(TokenType.INTEGER_LITERAL, lexeme, line, column);

            case 9: // IDENTIFIER (single uppercase letter only)
                // Check identifier length (max 31 characters)
                if (lexeme.length() > 31) {
                    String errorMsg = "Identifier exceeds maximum length of 31 characters";
                    errorHandler.add(lexeme, errorMsg, line, column);
                    return null;
                }
                
                if (isKeyword(lexeme))
                    return new Token(TokenType.KEYWORD, lexeme, line, column);

                symbolTable.add(lexeme, "identifier", line);
                return new Token(TokenType.IDENTIFIER, lexeme, line, column);

            case 10: // IDENTIFIER (uppercase + lowercase/digit/underscore)
                // Check identifier length (max 31 characters)
                if (lexeme.length() > 31) {
                    String errorMsg = "Identifier exceeds maximum length of 31 characters";
                    errorHandler.add(lexeme, errorMsg, line, column);
                    return null;
                }
                
                if (isKeyword(lexeme))
                    return new Token(TokenType.KEYWORD, lexeme, line, column);

                symbolTable.add(lexeme, "identifier", line);
                return new Token(TokenType.IDENTIFIER, lexeme, line, column);

            case 16: // STRING_LITERAL
                return new Token(TokenType.STRING_LITERAL, lexeme, line, column);

            case 25: // "false" (BOOLEAN_LITERAL)
                return new Token(TokenType.BOOLEAN_LITERAL, lexeme, line, column);

            case 27: // FLOAT_LITERAL with exponent
                return new Token(TokenType.FLOAT_LITERAL, lexeme, line, column);

            case 28: // FLOAT_LITERAL with decimal point
                // Check decimal places (max 6 digits after decimal point)
                int decimalPointIndex = lexeme.indexOf('.');
                if (decimalPointIndex != -1) {
                    // Find where the decimal digits end (before 'e' or 'E' if present)
                    int expIndex = lexeme.indexOf('e');
                    if (expIndex == -1) {
                        expIndex = lexeme.indexOf('E');
                    }
                    int endOfDecimals = (expIndex != -1) ? expIndex : lexeme.length();
                    int decimalDigits = endOfDecimals - decimalPointIndex - 1;
                    
                    if (decimalDigits > 6) {
                        String errorMsg = "Float literal exceeds maximum of 6 digits after decimal point";
                        errorHandler.add(lexeme, errorMsg, line, column);
                        return null;
                    }
                }
                return new Token(TokenType.FLOAT_LITERAL, lexeme, line, column);
        }

        return null;
    }

    private boolean isKeyword(String word) {
        String[] keywords = {
                "start", "finish", "loop", "condition",
                "declare", "output", "input",
                "function", "return", "break",
                "continue", "else"
        };

        return Arrays.asList(keywords).contains(word);
    }

    private char peek() {
        return input.charAt(position);
    }

    private void advance() {
        if (peek() == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        position++;
    }

    public void printStatistics() {
        System.out.println("Total Tokens: " + tokens.size());
        
        System.out.println("\n=== Token Type Counts ===");
        
        // Print counts for each token type
        int keywordCount = statistics.getOrDefault(TokenType.KEYWORD, 0);
        int identifierCount = statistics.getOrDefault(TokenType.IDENTIFIER, 0);
        int integerCount = statistics.getOrDefault(TokenType.INTEGER_LITERAL, 0);
        int floatCount = statistics.getOrDefault(TokenType.FLOAT_LITERAL, 0);
        int stringCount = statistics.getOrDefault(TokenType.STRING_LITERAL, 0);
        int booleanCount = statistics.getOrDefault(TokenType.BOOLEAN_LITERAL, 0);
        int commentCount = statistics.getOrDefault(TokenType.SINGLE_LINE_COMMENT, 0);
        
        if (keywordCount > 0) System.out.println("KEYWORD: " + keywordCount);
        if (identifierCount > 0) System.out.println("IDENTIFIER: " + identifierCount);
        if (integerCount > 0) System.out.println("INTEGER_LITERAL: " + integerCount);
        if (floatCount > 0) System.out.println("FLOAT_LITERAL: " + floatCount);
        if (stringCount > 0) System.out.println("STRING_LITERAL: " + stringCount);
        if (booleanCount > 0) System.out.println("BOOLEAN_LITERAL: " + booleanCount);
        if (commentCount > 0) System.out.println("SINGLE_LINE_COMMENT: " + commentCount);
        
        System.out.println("\n=== Skipped Items ===");
        System.out.println("Whitespaces Skipped: " + whitespacesSkipped);
        System.out.println("Comments Skipped: " + commentsSkipped);
    }

    // Expose symbol table so callers (e.g. Main) can inspect what the scanner recorded
    public SymbolTable getSymbolTable() {
        return symbolTable;
    }

    // Expose error handler so callers can inspect lexical errors
    public ErrorHandler getErrorHandler() {
        return errorHandler;
    }
}
