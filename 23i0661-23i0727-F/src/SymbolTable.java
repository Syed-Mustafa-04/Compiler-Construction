import java.util.*;

public class SymbolTable {
    private Map<String, SymbolInfo> symbols;

    public SymbolTable() {
        symbols = new LinkedHashMap<>();
    }

    // Now explicitly stores ONLY identifiers
    public void addIdentifier(String name, TokenType type, int line, int column) {

        // Safety check: only allow IDENTIFIER tokens
        if (type != TokenType.IDENTIFIER) {
            return;
        }

        if (symbols.containsKey(name)) {
            SymbolInfo info = symbols.get(name);
            info.incrementFrequency();
            info.addLocation(line, column);
        } else {
            SymbolInfo info = new SymbolInfo(name, TokenType.IDENTIFIER, line, column);
            symbols.put(name, info);
        }
    }

    public void printTable() {
        System.out.println("\n========== SYMBOL TABLE ==========");
        System.out.printf("%-20s %-10s %-10s %-20s\n",
                "Identifier", "Type", "Frequency", "First Occurrence");
        System.out.println("--------------------------------------------------------");

        for (SymbolInfo info : symbols.values()) {
            System.out.printf("%-20s %-10s %-10d Line: %d, Col: %d\n",
                    info.getName(),
                    info.getType(),
                    info.getFrequency(),
                    info.getFirstLine(),
                    info.getFirstColumn());
        }

        System.out.println("====================================\n");
    }

    public int getSize() {
        return symbols.size();
    }

    public static class SymbolInfo {
        private String name;
        private TokenType type;
        private int frequency;
        private int firstLine;
        private int firstColumn;
        private List<String> locations;

        public SymbolInfo(String name, TokenType type, int line, int column) {
            this.name = name;
            this.type = type;
            this.frequency = 1;
            this.firstLine = line;
            this.firstColumn = column;
            this.locations = new ArrayList<>();
            addLocation(line, column);
        }

        public void incrementFrequency() {
            frequency++;
        }

        public void addLocation(int line, int column) {
            locations.add("Line " + line + ", Col " + column);
        }

        public String getName() { return name; }
        public TokenType getType() { return type; }
        public int getFrequency() { return frequency; }
        public int getFirstLine() { return firstLine; }
        public int getFirstColumn() { return firstColumn; }
    }
}
