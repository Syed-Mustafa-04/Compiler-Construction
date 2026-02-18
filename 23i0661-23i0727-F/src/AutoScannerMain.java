import java.io.FileReader;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

public class AutoScannerMain {

    private String filename;
    private SymbolTable symbolTable;
    private ErrorHandler errorHandler;
    private Yylex lexer;

    // Statistics
    private int totalTokens = 0;
  
    private int commentCount = 0;

    private Map<TokenType, Integer> tokenStats;

    public AutoScannerMain(String filename) {

        this.filename = filename;
        this.symbolTable = new SymbolTable();
        this.errorHandler = new ErrorHandler();
        this.tokenStats = new HashMap<>();

        for (TokenType type : TokenType.values()) {
            tokenStats.put(type, 0);
        }
    }

    public void scan() {

        System.out.println("File: " + filename);

        try {

            lexer = new Yylex(new FileReader(filename));
            lexer.setSymbolTable(symbolTable);
            lexer.setErrorHandler(errorHandler);

            Token token;

            System.out.println("TOKENS OUTPUT:");
            System.out.println("--------------");

            while ((token = lexer.yylex()) != null) {

              

                /* Stop at EOF */
                if (token.getType() == TokenType.EOF) {
                    break;
                }

                /* Print token */
                System.out.println(token);

                /* Update statistics */
                if (token.getType() != TokenType.ERROR &&
                    token.getType() != TokenType.WHITESPACE) {

                    totalTokens++;

                    tokenStats.put(
                        token.getType(),
                        tokenStats.getOrDefault(token.getType(), 0) + 1
                    );

                    /* Add identifier to symbol table */
                    if (token.getType() == TokenType.IDENTIFIER) {

                        symbolTable.addIdentifier(
                            token.getLexeme(),
                            token.getType(),
                            token.getLine(),
                            token.getColumn()
                        );
                    }
                }
            }

            /* Get comment count from lexer */
            commentCount = lexer.getCommentCount();

            displayStatistics();
            symbolTable.printTable();
            errorHandler.printSummary();

        }
        catch (IOException e) {
            System.err.println("File error: " + e.getMessage());
        }
        catch (Exception e) {
            System.err.println("Scanner error: " + e.getMessage());
            e.printStackTrace();
        }
    }

   

    private void displayStatistics() {

        System.out.println("\n==========================================");
        System.out.println("           SCANNER STATS");
        System.out.println("==========================================");

        System.out.printf("%-30s: %d\n", "Total tokens processed", totalTokens);
        System.out.printf("%-30s: %d\n", "Lines processed", lexer.getLineCount());

        System.out.printf("%-30s: %d\n", "Comments removed", commentCount);

        System.out.println("\n------------------------------------------");
        System.out.println("TOKEN TYPE COUNTS:");
        System.out.println("------------------------------------------");

        TokenType[] specifiedTypes = {

            TokenType.INTEGER_LITERAL,
            TokenType.FLOAT_LITERAL,
            TokenType.IDENTIFIER,
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
        System.out.println("Example: java AutoScannerMain tests/test1.lang");
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
                System.err.println("File not found: " + filename);
                return;
            }

        }
        catch (Exception e) {
            System.err.println("File check error: " + e.getMessage());
        }

        AutoScannerMain scanner = new AutoScannerMain(filename);
        scanner.scan();
    }
}
