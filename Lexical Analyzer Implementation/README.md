Language Name: Why
File Extension: .y

All source programs must use the .y extension.

## Identifier Rules
Must start with an uppercase letter (A-Z)
Followed by lowercase letters (a-z), digits (0-9), or underscore (_)
Maximum length: 31 characters

### Valid Examples
Count
Total_sum
X
Value2024

### Invalid Examples
count          
2Value         
MyVariable     
VeryLongIdentifierNameMoreThan31Characters

### Literal Formats:

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

Format:
"([ ^"\\\n]|\\["\\ntr])*"

Enclosed in double quotes " ".

Supported Escape Sequences:

\", \\, \n, \t, \r 


Examples:

"Hello"
"Line1\nLine2"
"Unicode: \u0041"
"Multi
Line
String"

4. Boolean Literals
Format:
(true|false)

Example:
true
false


Comment Syntax
Single-line Comment
Format: ##[ ^\n]*
## This is a comment

### Sample Programs

1:

Count
Total123
X
Value9

123
0
-45
+789

3.14
0.5
-12.345
+9.8

true
false

"Hello"
"Compiler Project"
"Line1\nLine2"
"Tab\tTest"

## This is a comment
## Another comment here


2:

MainValue    100    3.14   true   "Hello"

Result42
-500   +7.25    false

"First String"     "Second\tString"

## This is a comment with numbers 123 4.56 true

Variable123   -999   0.0001

"Multiple\nLines\tHere"


3:
"Hello\nWorld"
"Tab\tSpace"
"Quote: \"Test\""
"Backslash: \\"
"Carriage\rReturn"

"Mix:\n\t\\\"End\""

true
false

123
45.67

## String test done


4:
hello          ## invalid (lowercase start if rule requires uppercase)
2Value         ## invalid (starts with digit)

12.34.56       ## invalid float
1.23456789     ## too many decimals

"Unclosed string

"Bad\qEscape"

True           ## invalid boolean (case sensitive)
FALSE

@bad
$wrong

## Valid comment

"Another bad string

5:
## First comment

## Second comment


## Comment with spaces      and tabs


Count       123      3.14


true       false


"Whitespace   Test"


## End comment


### Compilation & Execution Instructions

JFlex Scanner
Step 1 : Generate Lexer
 jflex Scanner.flex

Step 2 : Compile
javac *.java

Step 3 : Run
java AutoScannerMain ../test/test1.lang

Output Format:

Each token is displayed as:

<TOKEN_TYPE, "lexeme", Line: X, Col: Y>


Example:

<KEYWORD, "start", Line: 1, Col: 1>


### Team Members
Name	         Roll Number	Section
Saleha Muhammad   23i-0727	    	F
Syed Mustafa      23i-0661	    	F
