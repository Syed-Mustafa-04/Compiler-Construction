import java.util.*;

public class ErrorHandler {

    private final List<CompilationError> errors;
    private boolean hasErrors;

    public ErrorHandler() {
        errors = new ArrayList<>();
        hasErrors = false;
    }

    /* Main error reporting method */
    public void reportError(ErrorType type,
                            int line,
                            int column,
                            String lexeme,
                            String reason) {

        CompilationError error =
                new CompilationError(type, line, column, lexeme, reason);

        errors.add(error);
        hasErrors = true;

        printError(error);
    }

    /* Overloaded method (uses default reason) */
    public void reportError(ErrorType type,
                            int line,
                            int column,
                            String lexeme) {

        String reason = getDefaultReason(type);
        reportError(type, line, column, lexeme, reason);
    }

    /* Default messages */
    private String getDefaultReason(ErrorType type) {

        switch (type) {

            case INVALID_CHARACTER:
                return "Invalid character in source";

            case MALFORMED_INTEGER:
                return "Malformed integer literal";

            case MALFORMED_FLOAT:
                return "Malformed floating-point literal";

            case MALFORMED_STRING:
                return "Unterminated or invalid string literal";

            case MALFORMED_CHARACTER:
                return "Invalid character literal format";

            case INVALID_IDENTIFIER:
                return "Invalid identifier format";

            case UNCLOSED_COMMENT:
                return "Comment not closed";

            case IDENTIFIER_TOO_LONG:
                return "Identifier exceeds 31 characters";

            default:
                return "Lexical error";
        }
    }

    /* Print single error */
    private void printError(CompilationError error) {

        System.err.println("========================================");
        System.err.println("LEXICAL ERROR: " + error.getType());
        System.err.println("Location: Line " +
                error.getLine() +
                ", Column " +
                error.getColumn());
        System.err.println("Lexeme: \"" + error.getLexeme() + "\"");
        System.err.println("Reason: " + error.getReason());
        System.err.println("========================================\n");
    }

    /* Has any error occurred? */
    public boolean hasErrors() {
        return hasErrors;
    }

    /* Print summary */
    public void printSummary() {

        if (!hasErrors) {
            System.out.println("\n✅ No lexical errors found.");
            return;
        }

        System.out.println("\n========== ERROR SUMMARY ==========");
        System.out.println("Total errors: " + errors.size());

        Map<ErrorType, Integer> counts = new HashMap<>();

        for (CompilationError e : errors) {
            counts.put(e.getType(),
                    counts.getOrDefault(e.getType(), 0) + 1);
        }

        for (Map.Entry<ErrorType, Integer> entry : counts.entrySet()) {
            System.out.println(entry.getKey() + ": " + entry.getValue());
        }

        System.out.println("====================================\n");
    }

    /* Clear errors */
    public void clear() {
        errors.clear();
        hasErrors = false;
    }

    /* ================= ERROR TYPES ================= */

    public enum ErrorType {

        INVALID_CHARACTER,

        MALFORMED_INTEGER,
        MALFORMED_FLOAT,
        MALFORMED_STRING,
        MALFORMED_CHARACTER,

        INVALID_IDENTIFIER,
        IDENTIFIER_TOO_LONG,

        UNCLOSED_COMMENT
    }

    /* ============== ERROR RECORD ================= */

    public static class CompilationError {

        private final ErrorType type;
        private final int line;
        private final int column;
        private final String lexeme;
        private final String reason;

        public CompilationError(ErrorType type,
                                int line,
                                int column,
                                String lexeme,
                                String reason) {

            this.type = type;
            this.line = line;
            this.column = column;
            this.lexeme = lexeme;
            this.reason = reason;
        }

        public ErrorType getType() {
            return type;
        }

        public int getLine() {
            return line;
        }

        public int getColumn() {
            return column;
        }

        public String getLexeme() {
            return lexeme;
        }

        public String getReason() {
            return reason;
        }
    }
}
