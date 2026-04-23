#ifndef STACK_H
#define STACK_H

#include <vector>
#include <string>
using namespace std;

class Stack {
public:
    vector<pair<string, int>> elements; // symbol, state

    void push(const string& symbol, int state);
    void pop();
    int topState() const;
    string topSymbol() const;
    bool empty() const;
    string toString() const;
};

#endif
