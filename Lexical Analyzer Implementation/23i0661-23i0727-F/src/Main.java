public class Main {
    public static void main(String[] args) {
<<<<<<< HEAD:A1/23i0661-23i0727-F/src/Main.java
        // Read testInput from test1.lang file
        String testInput = readFile("test5.lang");
        
=======
        // Read testInput from test file
        String testInput = readFile("test\\test_simple.lang");

>>>>>>> 8217ee31739612a69127f2fd6fbceaffc93a5549:23i0661-23i0727-F/src/Main.java
        ManualScanner scanner = new ManualScanner(testInput);
        java.util.List<Token> tokens = scanner.scan();

        System.out.println("=== Generated Tokens ===");
        for (Token token : tokens) {
            System.out.println(token);
        }
        
<<<<<<< HEAD:A1/23i0661-23i0727-F/src/Main.java
        System.out.println("\n=== Lexical Errors ===");
        ErrorHandler errorHandler = scanner.getErrorHandler();
        if (errorHandler.getErrorCount() > 0) {
            errorHandler.print();
=======
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
>>>>>>> 8217ee31739612a69127f2fd6fbceaffc93a5549:23i0661-23i0727-F/src/Main.java
        } else {
            System.out.println("No errors found.");
        }
        
        System.out.println("\n=== Statistics ===");
        scanner.printStatistics();
        
        System.out.println("\n=== Symbol Table ===");
        // Use the symbol table populated by the scanner
        SymbolTable symbolTable = scanner.getSymbolTable();
<<<<<<< HEAD:A1/23i0661-23i0727-F/src/Main.java
        symbolTable.print();
=======
        symbolTable.printTable();
>>>>>>> 8217ee31739612a69127f2fd6fbceaffc93a5549:23i0661-23i0727-F/src/Main.java
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
