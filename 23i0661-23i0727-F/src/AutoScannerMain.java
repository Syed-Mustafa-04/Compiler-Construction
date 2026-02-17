import java.io.FileReader;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

public class AutoScannerMain {

    private String filename;
    private SymbolTable symbolTable;
    private ErrorHandler errorHandler;
    private Yylex lexer;

    // Statistics tracking
    private int totalTokens = 0;
    private int totalLines = 0;
    private int commentCount = 0;
    private Map<TokenType, Integer> tokenStats;

    public AutoScannerMain(String filename) {
        this.filename = filename;
        this.symbolTable = new SymbolTable();
        this.errorHandler = new ErrorHandler();
        this.tokenStats = new HashMap<>();
        // Initialize all token types with 0 count
        for (TokenType type : TokenType.values()) {
            tokenStats.put(type, 0);
        }
    }

    public void scan() {

        System.out.println("\n==========================================");
        System.out.println("   JFlex LEXICAL SCANNER - AUTO SCANNER");
        System.out.println("==========================================");
        System.out.println("File: " + filename);
        System.out.println("==========================================\n");

        try {
            lexer = new Yylex(new FileReader(filename));
            lexer.setSymbolTable(symbolTable);
            lexer.setErrorHandler(errorHandler);

            Token token;
            int tokenCount = 0;

            System.out.println("TOKENS OUTPUT:");
            System.out.println("--------------");

            while ((token = lexer.yylex()) != null) {

                // Count single-line comments
                if (token.getType() == TokenType.SINGLE_LINE_COMMENT) {
                    commentCount++;
                }

                // Count new lines
                if (token.getLexeme().contains("\n")) {
                    totalLines += countNewlines(token.getLexeme());
                }

                // Break on EOF
                if (token.getType() == TokenType.EOF) {
                    break;
                }

                // Print the token (including errors)
                System.out.println(token);

                // Only update statistics for non-error tokens
                if (token.getType() != TokenType.ERROR &&
                    token.getType() != TokenType.WHITESPACE &&
                    token.getType() != TokenType.SINGLE_LINE_COMMENT) {

                    totalTokens++;
                    tokenStats.put(token.getType(),
                        tokenStats.getOrDefault(token.getType(), 0) + 1);

                    // Add identifiers to symbol table
                    if (token.getType() == TokenType.IDENTIFIER) {
                        symbolTable.addIdentifier(token.getLexeme(),
                                                  token.getType(),
                                                  token.getLine(),
                                                  token.getColumn());
                    }
                }
            }


            displayStatistics();
            symbolTable.printTable();
            errorHandler.printSummary();

        } catch (IOException e) {
            System.err.println("Error reading file: " + e.getMessage());
        } catch (Exception e) {
            System.err.println("Error during scanning: " + e.getMessage());
            e.printStackTrace();
        }
    }

    private int countNewlines(String text) {
        int count = 0;
        for (int i = 0; i < text.length(); i++) {
            if (text.charAt(i) == '\n') {
                count++;
            }
        }
        return count;
    }

    private void displayStatistics() {

        System.out.println("\n==========================================");
        System.out.println("           SCANNER STATISTICS");
        System.out.println("==========================================");

        System.out.printf("%-30s: %d\n", "Total tokens processed", totalTokens);
        System.out.printf("%-30s: %d\n", "Lines processed", totalLines + 1);
        System.out.printf("%-30s: %d\n", "Comments removed", commentCount);

        System.out.println("\n------------------------------------------");
        System.out.println("TOKEN TYPE COUNTS (Only specified token types):");
        System.out.println("------------------------------------------");

        TokenType[] specifiedTypes = {
        	    TokenType.INTEGER_LITERAL,
        	    TokenType.FLOAT_LITERAL,
        	    TokenType.IDENTIFIER,
        	    TokenType.SINGLE_LINE_COMMENT,
        	    TokenType.STRING_LITERAL,
        	    TokenType.BOOLEAN_LITERAL,
        	    TokenType.WHITESPACE,
        	    TokenType.EOF,
        	    TokenType.ERROR
        	};


        for (TokenType type : specifiedTypes) {
            int count = tokenStats.getOrDefault(type, 0);
            if (count > 0 || type == TokenType.ERROR) {
                System.out.printf("  %-20s: %d\n", type, count);
            }
        }

        System.out.println("==========================================\n");
    }

    public static void printUsage() {
        System.out.println("==========================================");
        System.out.println("           AUTO SCANNER MAIN");
        System.out.println("==========================================");
        System.out.println("Usage: java AutoScannerMain <filename>");
        System.out.println();
        System.out.println("Example:");
        System.out.println("  java AutoScannerMain tests/test1.lang");
        System.out.println("==========================================");
    }

    public static void main(String[] args) {

        if (args.length == 0) {
            printUsage();
            return;
        }

        String filename = args[0];

        try {
            java.io.File file = new java.io.File(filename);
            if (!file.exists()) {
                System.err.println("Error: File '" + filename + "' not found!");
                return;
            }
        } catch (Exception e) {
            System.err.println("Error checking file: " + e.getMessage());
        }

        AutoScannerMain scanner = new AutoScannerMain(filename);
        scanner.scan();
    }
}