#include <iostream>
#include <fstream>
#include <string>

#include "grammar.h"
#include "first_follow.h"
#include "parser.h"

using namespace std;


//  Clear an output file before writing
static void clearFile(const string& path) {
    ofstream f(path, ios::trunc);
}

int main(int argc, char* argv[]) {

    //Determine input grammar file
    string grammarFile = "input/grammar2.txt";
    bool verbose = false;
    
    if (argc >= 2) grammarFile = argv[1];
    if (argc >= 3) {
        string arg2 = argv[2];
        if (arg2 == "-v" || arg2 == "--verbose") {
            verbose = true;
        }
    }

    // ── Output files
    const string outGrammar    = "output/grammar_transformed.txt";
    const string outFirstFollow = "output/first_follow_sets.txt";
    const string outParsingTable = "output/parsing_table.txt";

    clearFile(outGrammar);
    clearFile(outFirstFollow);
    clearFile(outParsingTable);

    //TASK 1.1  Load CFG 
    Grammar grammar;
    if (!grammar.loadFromFile(grammarFile)) {
        cerr << "\n[FATAL] Could not load grammar. Exiting.\n";
        return 1;
    }

    if (verbose) {
        grammar.display("Task 1.1  -  Original Grammar (as loaded)");
    } else {
        cout << "Task 1.1: Grammar loaded successfully\n";
    }
    grammar.writeToFile(outGrammar, "Task 1.1  -  Original Grammar");

    // TASK 1.2  Left Factoring
    Grammar afterLF = grammar;
    afterLF.applyLeftFactoring();

    if (verbose) {
        afterLF.display("Task 1.2  -  After Left Factoring");
    } else {
        cout << "Task 1.2: Left factoring completed\n";
    }
    afterLF.writeToFile(outGrammar, "Task 1.2  -  After Left Factoring");

    //TASK 1.3  Left Recursion Removal
    Grammar afterLR = afterLF;
    afterLR.removeLeftRecursion();

    if (verbose) {
        afterLR.display("Task 1.3  -  After Left Recursion Removal");
    } else {
        cout << "Task 1.3: Left recursion removal completed\n";
    }
    afterLR.writeToFile(outGrammar, "Task 1.3  -  After Left Recursion Removal");

    //TASK 1.4 & 1.5  FIRST / FOLLOW Sets
    FirstFollow ff(afterLR);
    ff.computeFirst();
    ff.computeFollow();

    if (verbose) {
        ff.display();
    } else {
        cout << "Task 1.4: FIRST sets computed\n";
        cout << "Task 1.5: FOLLOW sets computed\n";
    }
    ff.writeToFile(outFirstFollow);

    //TASK 1.6  LL(1) Parsing Table Construction
    Parser parser(afterLR, ff);
    
    if (verbose) {
        parser.display();
    } else {
        cout << "Task 1.6: LL(1) Parsing table constructed";
        if (parser.isLL1()) {
            cout << " [LL(1): YES]\n";
        } else {
            cout << " [LL(1): NO - " << parser.getConflictCount() << " conflict(s)]\n";
        }
    }
    parser.writeToFile(outParsingTable);

    cout << "\n  Output files updated:\n";
    cout << "    - " << outGrammar     << "\n";
    cout << "    - " << outFirstFollow << "\n";
    cout << "    - " << outParsingTable << "\n";

    // TASK 1.7 & 1.8  LL(1) Parsing with Trace
    cout << "\nTask 1.7 & 1.8: Parsing with error recovery\n";

    int validCount = 0, validAccepted = 0;
    int errorCount = 0, errorAccepted = 0;
    int edgeCount = 0, edgeAccepted = 0;

    //Parsing: Valid Inputs
    {
        ifstream validFile("input/input_valid_G2.txt");
        if (validFile.is_open()) {
            string line;
            bool firstValidTrace = true;
            
            while (getline(validFile, line)) {
                if (line.empty() || line[0] == '#') continue;
                
                parser.resetErrors();
                validCount++;
                
                string traceFile = "";
                if (firstValidTrace) {
                    traceFile = "output/parsing_trace1.txt";
                    firstValidTrace = false;
                }
                
                bool result = parser.parseWithTrace(line, traceFile);
                if (result) validAccepted++;
            }
            validFile.close();
        }
    }

    //Parsing: Error Inputs 
    {
        ifstream errorFile("input/input_errors_G2.txt");
        if (errorFile.is_open()) {
            string line;
            bool firstErrorTrace = true;
            
            while (getline(errorFile, line)) {
                if (line.empty() || line[0] == '#') continue;
                
                parser.resetErrors();
                errorCount++;
                
                string traceFile = "";
                if (firstErrorTrace) {
                    traceFile = "output/parsing_trace2.txt";
                    firstErrorTrace = false;
                }
                
                bool result = parser.parseWithTrace(line, traceFile);
                if (result) errorAccepted++;
            }
            errorFile.close();
        }
    }

    //Parsing: Edge Cases
    {
        ifstream edgeFile("input/input_edge_cases_G2.txt");
        if (edgeFile.is_open()) {
            string line;
            bool firstEdgeTrace = true;
            
            while (getline(edgeFile, line)) {
                if (line.empty() || line[0] == '#') continue;
                
                parser.resetErrors();
                edgeCount++;
                
                string traceFile = "";
                if (firstEdgeTrace) {
                    traceFile = "output/parsing_trace3.txt";
                    firstEdgeTrace = false;
                }
                
                bool result = parser.parseWithTrace(line, traceFile);
                if (result) edgeAccepted++;
            }
            edgeFile.close();
        }
    }

    // Print parsing summary
    if (validCount > 0) {
        cout << "    - Valid inputs:   " << validAccepted << "/" << validCount << " accepted\n";
    }
    if (errorCount > 0) {
        cout << "    - Error inputs:   " << errorAccepted << "/" << errorCount << " recovered\n";
    }
    if (edgeCount > 0) {
        cout << "    - Edge cases:     " << edgeAccepted << "/" << edgeCount << " accepted\n";
    }

    cout << "\n  Trace files generated:\n";
    cout << "    - output/parsing_trace1.txt\n";
    cout << "    - output/parsing_trace2.txt\n";
    cout << "    - output/parsing_trace3.txt\n";

    //TASK 2.5  Parse Tree Generation (for valid inputs) ----
    cout << "\nTask 2.5: Parse Tree Generation (for successfully parsed inputs)\n";

    // Clear parse trees output file
    clearFile("output/parse_trees.txt");

    int parseTreeCount = 0;
    {
        ifstream validFile("input/input_valid_G2.txt");
        if (validFile.is_open()) {
            string line;
            int testNum = 1;
            
            while (getline(validFile, line)) {
                if (line.empty() || line[0] == '#') continue;
                
                parser.resetErrors();
                
                // Parse the input
                bool result = parser.parseWithTrace(line, "");
                
                // If parsing succeeded, generate parse tree
                if (result) {
                    parseTreeCount++;
                    
                    // Use the actual parse tree built by the parser
                    const ParseTree& tree = parser.getParseTree();
                    
                    // Write tree to file
                    tree.writeToFile("output/parse_trees.txt");
                    
                    if (verbose) {
                        cout << "  Parse tree " << parseTreeCount << " (Input: " << line << ")\n";
                        tree.display();
                    }
                }
                
                testNum++;
            }
            validFile.close();
        }
    }

    cout << "    - Parse trees generated for " << parseTreeCount << " successful parse(s)\n";
    cout << "    - Output file: output/parse_trees.txt\n";

    return 0;
}
