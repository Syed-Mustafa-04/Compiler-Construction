public class Main {
    public static void main(String[] args) {
        // Read testInput from test file
        String testInput = readFile("test\\test_simple.lang");

        ManualScanner scanner = new ManualScanner(testInput);
        java.util.List<Token> tokens = scanner.scan();

        System.out.println("=== Generated Tokens ===");
        for (Token token : tokens) {
            System.out.println(token);
        }
        
        System.out.println("\n=== Comments Found ===");
        java.util.List<ManualScanner.CommentInfo> comments = scanner.getComments();
        if (comments.isEmpty()) {
            System.out.println("No comments found.");
        } else {
            for (ManualScanner.CommentInfo comment : comments) {
                System.out.println(comment);
            }
        }
        
        System.out.println("\n=== Lexical Errors ===");
        ErrorHandler errorHandler = scanner.getErrorHandler();
        if (errorHandler.hasErrors()) {
            errorHandler.printSummary();
        } else {
            System.out.println("No errors found.");
        }
        
        System.out.println("\n=== Statistics ===");
        scanner.printStatistics();
        
        System.out.println("\n=== Symbol Table ===");
        // Use the symbol table populated by the scanner
        SymbolTable symbolTable = scanner.getSymbolTable();
        symbolTable.printTable();
    }

    private static String readFile(String filename) {
        try {
            return new String(java.nio.file.Files.readAllBytes(java.nio.file.Paths.get(filename)));
        } catch (java.io.IOException e) {
            System.err.println("Error reading file: " + filename);
            e.printStackTrace();
            return "";
        }
    }
}
