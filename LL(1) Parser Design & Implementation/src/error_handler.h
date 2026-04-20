#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <string>

using namespace std;
 
class ErrorHandler {
public:
    int errorCount = 0;
    void reportError(int line, int col,
                     const string& expected,
                     const string& found);
    void reportWarning(const string& msg);
    bool hasErrors() const { return errorCount > 0; }
};
#endif
