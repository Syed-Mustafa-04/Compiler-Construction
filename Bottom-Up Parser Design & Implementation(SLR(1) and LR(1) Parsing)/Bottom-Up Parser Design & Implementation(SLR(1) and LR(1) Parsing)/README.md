# SLR(1) and LR(1) Parser Implementation

## Team Members
- Roll Number 1: 23i0727
- Roll Number 2: 23i0661
- Section: F

## Language
C++ (C++11 standard)

## Project Overview
This project implements both SLR(1) and LR(1) bottom-up parsers with full support for:
- Context-Free Grammar (CFG) parsing from files
- Grammar augmentation with new start symbol
- LR(0) and LR(1) item construction
- Canonical collection generation
- Parsing table construction
- Stack-based shift-reduce parsing
- Parse tree generation
- Detailed parsing traces

## Compilation Instructions

### Prerequisites
- GCC or Clang C++ compiler with C++11 support
- GNU Make

### Build Project
```bash
cd d:\23i0727-23i0661-F
make clean
make
```

This will generate the executable `slr_parser.exe` in the project root directory.

## Execution Instructions

### Basic Usage
```bash
.\slr_parser.exe <grammar_file> <input_file>
```

### Examples

**Test with Grammar 1 (Simple Expression with Addition and Multiplication):**
```bash
.\slr_parser.exe input\grammar1.txt input\input_valid.txt
```

**Test with Grammar 2 (Expression with Parentheses):**
```bash
.\slr_parser.exe input\grammar2.txt input\input_valid.txt
```

**Test with Grammar 3 (Demonstrates SLR(1) vs LR(1) Difference):**
```bash
.\slr_parser.exe input\grammar3.txt input\input_valid.txt
```

**Test with Invalid Input:**
```bash
.\slr_parser.exe input\grammar1.txt input\input_invalid.txt
```

## Input File Format

### Grammar File Format
Each production rule should be on a separate line with the following format:
```
NonTerminal -> production1 | production2 | ...
```

**Rules:**
- Non-terminals: Multi-character names starting with uppercase (e.g., Expr, Term, Factor)
- Terminals: Lowercase letters, operators, keywords (e.g., id, +, *, (, ))
- Arrow symbol: Must be ` -> ` (with spaces)
- Alternatives: Separated by ` | ` (with spaces)
- Epsilon: Use `epsilon` or `@`

**Example (grammar1.txt):**
```
Expr -> Expr + Term | Term
Term -> Term * Factor | Factor
Factor -> ( Expr ) | id
```

### Input String Format
Each token/symbol should be separated by whitespace on a single line.

**Example (input_valid.txt):**
```
id + id * id
```

## Output Files

The program generates the following output files in the `output/` directory:

| File | Description |
|------|-------------|
| `augmented_grammar.txt` | Original grammar with augmented start symbol |
| `slr_items.txt` | LR(0) items and canonical collection (SLR(1)) |
| `slr_parsing_table.txt` | Complete SLR(1) parsing table with ACTION and GOTO entries |
| `slr_trace.txt` | Step-by-step SLR(1) parsing trace |
| `lr1_items.txt` | LR(1) items with lookaheads and canonical collection |
| `lr1_parsing_table.txt` | Complete LR(1) parsing table |
| `lr1_trace.txt` | Step-by-step LR(1) parsing trace |
| `parse_trees.txt` | Parse tree for successfully parsed input (SLR(1)) |
| `comparison.txt` | Comparison analysis between SLR(1) and LR(1) |

## Program Output

The program will display:
```
=== SLR(1) Parser ===
Parsing successful!
Input accepted by SLR(1)!

=== LR(1) Parser ===
Input accepted by LR(1)!
```

Or if parsing fails:
```
=== SLR(1) Parser ===
Error: No action for state X and symbol Y
Input rejected by SLR(1)!
```

## Test Grammars

### Grammar 1 (Simple Expression)
```
Expr -> Expr + Term | Term
Term -> Term * Factor | Factor
Factor -> ( Expr ) | id
```
- **Valid Input**: `id + id * id`
- **Invalid Input**: `id + id +` (incomplete)

### Grammar 2 (Expression with Parentheses)
Same as Grammar 1 - tests nested expressions and operator precedence

### Grammar 3 (SLR(1) vs LR(1) - Shift/Reduce Conflict)
```
Start -> L = R | R
L -> * R | id
R -> L
```
- **SLR(1)**: ❌ REJECTED (shift/reduce conflict)
- **LR(1)**: ✅ ACCEPTED (lookahead resolves conflict)
- **Valid Input**: `id = id`

## Key Features

✅ **Complete LR(0) Implementation**
- CLOSURE algorithm with proper epsilon handling
- GOTO function for state transitions
- Canonical collection generation

✅ **SLR(1) Parser**
- Parsing table construction using FOLLOW sets
- Conflict detection and reporting
- Shift-reduce parsing with stack

✅ **LR(1) Parser**
- Lookahead-based item construction
- More precise parsing table
- Resolves conflicts that SLR(1) cannot handle

✅ **Parse Tree Generation**
- Tree construction during parsing
- Hierarchical display of reductions
- Text-based tree output

✅ **Detailed Traces**
- Step-by-step parsing actions
- Stack state at each step
- Remaining input visualization

## Known Limitations

- Single start symbol per grammar
- No support for empty grammar (at least one production required)
- Input file must contain valid tokens (whitespace-separated)
- Parse trees generated only for successfully parsed inputs

## Grammar File Examples

### Example 1: Simple Arithmetic
```
E -> E + T | T
T -> T * F | F
F -> id | ( E )
```

### Example 2: Assignment Statements
```
S -> id = E | E
E -> E + E | E * E | id | ( E )
```

### Example 3: If-Else Statements
```
Stmt -> if E then Stmt | if E then Stmt else Stmt | other
E -> id
```

## Compilation Options

To enable verbose output or debugging:
```bash
make CXXFLAGS="-std=c++11 -Wall -Wextra -g"
```

## Performance Notes

- SLR(1): Faster, uses fewer states for many grammars
- LR(1): More powerful, handles complex grammars but generates more states
- For grammar3: LR(1) generates the same number of states but with better lookahead handling

## Troubleshooting

**Problem**: `slr_parser.exe: command not found`
- **Solution**: Ensure you're in the correct directory and run `make` first

**Problem**: `Error: No action for state X and symbol Y`
- **Solution**: This is expected for invalid inputs or grammars with SLR(1) conflicts. Use grammar3 to see LR(1)'s advantage.

**Problem**: Parse tree file is empty
- **Solution**: Parse trees are only generated for successfully parsed inputs. Test with valid input strings.

## Additional Resources

For more information on LR parsing, see:
- Dragon Book: "Compilers: Principles, Techniques, and Tools"
- "Engineering a Compiler" by Keith D. Cooper and Linda Torczon
- Algorithm references in source code comments
