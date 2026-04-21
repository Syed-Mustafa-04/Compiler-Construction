#ifndef STACK_H
#define STACK_H

#include <vector>
#include <string>

class Stack {
public:
    std::vector<std::pair<std::string, int>> elements; // symbol, state

    void push(const std::string& symbol, int state);
    void pop();
    int topState() const;
    std::string topSymbol() const;
    bool empty() const;
    std::string toString() const;
};

#endif