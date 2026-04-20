## Team Members

| Roll Number | Section |
|-------------|---------|
| 23i0661     | F       |
| 23i0727     | F       |

---

## Programming Language

**C++** (C++17 standard)

---

## Compilation Instructions

### Prerequisites
- **Compiler:** GCC 7.0+ with C++17 support (or any compatible C++ compiler)
- **Build tool:** GNU Make
- **Platform:** Windows (MinGW)

### Build Steps

1. **Navigate to the project directory:**
   bash
   cd path/to/23i0661-23i0727-F
   

2. **Compile the project:**
   bash
   make all
   
   
   This will:
   - Create necessary directories (`input/`, `output/`, `docs/`)
   - Compile all source files with C++17 standards
   - Link object files to generate `parser.exe`
   - Display "Build successful → ./parser.exe" on completion

3. **Clean build artifacts (optional):**
   bash
   make clean


## Execution Instructions

### Basic Execution

**Default usage** (uses `input/grammar1.txt`):
```bash
./parser.exe
```

**Specify custom grammar file:**
bash
./parser.exe input/grammar2.txt

## Input File Format Specification

### Grammar File Format (`.txt`)

**File Structure:**
- One production rule per line
- Comments start with `#` and extend to end of line
- Blank lines are ignored

**Production Format:**
NonTerminal -> production1 | production2 | production3


**Components:**
- **Non-Terminals:** Multi-character identifiers starting with uppercase letter (e.g., `Expr`, `Term`, `Factor`)
  - Single-character non-terminals (E, T, F) are **NOT allowed**
- **Terminals:** Lowercase letters, operators, keywords, numbers (e.g., `id`, `num`, `+`, `*`, `:=`, `then`)
- **Arrow:** Use literal `->` (with spaces recommended for readability)
- **Separator:** Use `|` to separate alternative productions
- **Epsilon:** Represented as `epsilon` or `@` (for empty production)

**Example Grammar File:**
```
# Grammar 1 – Expression Grammar
# Non-terminals: Expr, Term, Factor
# Terminals: +, *, (, ), ids
Expr -> Expr + Term | Term
Term -> Term * Factor | Factor
Factor -> ( Expr ) | ids

### Input String File Format

Input strings to be parsed are typically embedded in the grammar file or provided separately with the following format:

Format:
token1 token2 token3 ... tokenN $

Rules:
- Tokens are space-separated
- Each string ends with `$` (end-of-input marker)
- One string per line
- Comments with `#` are allowed

Example Input String:**

ids + ids * ids $
( ids + ids ) * ids $
```
## Grammar File Format Details

### Task 1.1: Original Grammar (Input)
Grammar files should contain Context-Free Grammar (CFG) rules in the format specified above.

### Task 1.2: Left Factoring
After left factoring, common prefixes are extracted. For example:
```
Original:  A -> a b | a c
Factored:  A -> a A'
           A' -> b | c
```

### Task 1.3: Left Recursion Removal

**Direct Left Recursion:**
```
Original:  E -> E + T | T
Transformed: E -> T E'
             E' -> + T E' | ε
```

**Indirect Left Recursion:** Eliminated through systematic non-terminal ordering and substitution.

### Task 1.4: FIRST Sets
Computed according to:
1. FIRST(terminal) = {terminal}
2. If X → ε, then ε ∈ FIRST(X)
3. For X → Y₁Y₂...Yₙ, include FIRST(Y₁)-{ε}, and if Y₁ derives ε, include FIRST(Y₂)-{ε}, etc.

### Task 1.5: FOLLOW Sets
Computed according to:
1. FOLLOW(start_symbol) contains $
2. If A → αBβ, add FIRST(β)-{ε} to FOLLOW(B)
3. If A → αB or A → αBβ where β ⇒* ε, add FOLLOW(A) to FOLLOW(B)

---

## Sample Grammar and Input Files Explanation

### Sample 1: Grammar1.txt - Classic Expression Grammar

**File Content:**
```
# Grammar 1 – Classic Expression Grammar
# Has direct left recursion in Expr and Term
# Also requires left factoring (common prefix in Factor)
Expr -> Expr + Term | Term
Term -> Term * Factor | Factor
Factor -> ( Expr ) | ids
```

**Description:**
- Classic arithmetic expression grammar
- Contains **direct left recursion** in `Expr` (E → E + T) and `Term` (T → T * F)
- Demonstrates left recursion removal transformation
- `ids` represents identifiers

**Sample Input Strings:**
```
ids + ids $
ids + ids * ids $
( ids ) $
( ids + ids ) * ids $
```

---

### Sample 2: Grammar2.txt - Statement Grammar with Left Factoring

**File Content:**
```
# Grammar 2 – Statement grammar needing left factoring
# if-then and if-then-else share common prefix
Stmt -> if Cond then Stmt else Stmt | if Cond then Stmt | assign
Cond -> id rel id | id
Assign -> id := Expr
Expr -> Expr + Term | Term
Term -> Factor
Factor -> id | num
```

**Description:**
- Statement-level grammar with if-then-else constructs
- Demonstrates **left factoring** (if-then and if-then-else share prefix `if Cond then Stmt`)
- Contains terminals like `if`, `then`, `else`, `:=` (assignment operator)
- Applications: Conditional statement parsing

---

### Sample 3: Grammar3.txt - Indirect Left Recursion

**File Content:**
```
# Grammar 3 – Indirect left recursion
# Start -> Alpha a | b
# Alpha -> Alpha c | Start d | epsilon
# Indirect: Alpha -> Start d  and  Start -> Alpha a  creates a cycle
Start -> Alpha a | b
Alpha -> Alpha c | Start d | epsilon
```

**Description:**
- Demonstrates **indirect left recursion** through mutual recursion between `Start` and `Alpha`
- Indirect left recursion: Start → Alpha a, Alpha → Start d creates a dependency cycle
- Epsilon production in `Alpha` (used for base case)
- Complex transformation required to make grammar LL(1)

## Known Limitations

1. **Single-Character Non-Terminals Not Supported:**
   - Non-terminals must be multi-character identifiers starting with uppercase
   - Use `Expr` instead of `E`, `Term` instead of `T`

3. **LL(1) Grammar Requirement:**
   - Input grammar must be transformable to LL(1) form
   - Non-LL(1) grammars may result in conflicts in the parsing table
   - Ambiguous grammars require additional disambiguation rules

4. **Token Length:**
   - Tokens must contain valid characters (alphanumerics, operators)
   - Whitespace is the token delimiter

5. **File Path Constraints:**
   - Input grammar files must be in `input/` directory
   - Output files are always written to `output/` directory
   - Paths are relative to the executable location

6. **Parsing Trace Size:**
   - Very long input strings may produce large trace files
   - No built-in size limits on trace output

7. **Grammar File Comments:**
   - Comments must start with `#` at the beginning of a line
   - Inline comments within a production are not supported



