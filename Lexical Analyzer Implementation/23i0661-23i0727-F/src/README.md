Language Name: Why
File Extension: .y

All source programs must use the .y extension.

Identifier Rules

Identifiers must follow these rules:

Must start with an uppercase letter (A-Z)

Followed by lowercase letters (a-z), digits (0-9), or underscore (_)

Maximum length: 31 characters

Valid Identifiers:
Count
Total_sum
X
Value2024

Invalid Identifiers
count          
2Value         
MyVariable     
VeryLongIdentifierNameMoreThan31Characters

Literal Formats:
1. Integer Literals

Format:

[+|-]?[0-9]+


Examples:

42
+100
-567
0


Invalid:

12.34
1,000

2. Floating-Point Literals

Format:

[+|-]?[0-9]+\.[0-9]{1,6}([eE][+|-]?[0-9]+)?


Examples:

3.14
+2.5
-0.123456
1.5e10
2.0E-3


Invalid:

3.
.14
1.2345678   (more than 6 decimals)

3. String Literals

Enclosed in double quotes " ".

Supported Escape Sequences:

\"   \\   \n   \t   \r   \uXXXX


Examples:

"Hello"
"Line1\nLine2"
"Unicode: \u0041"
"Multi
Line
String"


5. Boolean Literals
true
false


Comment Syntax
Single-line Comment
## This is a comment

Sample Programs
1:  

Count
Count = 10
output Count
finish

Sample Program 2 
start
declare X
X = 5

loop (X > 0) {
    output X
    X--
}

finish

Sample Program 3 
start

function Add {
    declare A
    declare B
    return A + B
}

declare Result
Result = 10 + 20
output Result

finish

Compilation & Execution Instructions
Manual Scanner
Compile
javac src/*.java

Run
java src.ManualScanner tests/test1.nova

JFlex Scanner
Step 1 : Generate Lexer
 jflex Scanner.flex

Step 2 : Compile
javac *.java

Step 3 : Run
java AutoScannerMain ../test/test1.lang

Output Format

Each token is displayed as:

<TOKEN_TYPE, "lexeme", Line: X, Col: Y>


Example:

<KEYWORD, "start", Line: 1, Col: 1>

Features Implemented

Manual DFA-based scanner

JFlex-based scanner

Nested multi-line comment support

Multi-line string support

Unicode escape handling

Symbol Table implementation

Error detection and recovery

Token statistics display

Longest match principle

Pattern priority enforcement

Team Members
Name	         Roll Number	Section
Saleha Muhammad   23i-0727	    	F
Syed Mustafa      23i-0661	    	F
