// error_handler.cpp  –  stub
#include "error_handler.h"
#include <iostream>

using namespace std;
 
void ErrorHandler::reportError(int line, int col,
                                const string& expected,
                                const string& found) {
    ++errorCount;
    cerr << "[ERROR] Line " << line << ", Col " << col
              << ": Expected '" << expected
              << "', found '" << found << "'.\n";
}
void ErrorHandler::reportWarning(const string& msg) {
    cout << "[WARNING] " << msg << "\n";
}
 
