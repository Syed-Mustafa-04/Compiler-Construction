#include "stack.h"
#include <sstream>
using namespace std;

void Stack::push(const string& symbol, int state) {
    elements.push_back({symbol, state});
}

void Stack::pop() {
    if (!elements.empty()) {
        elements.pop_back();
    }
}

int Stack::topState() const {
    return elements.empty() ? -1 : elements.back().second;
}

string Stack::topSymbol() const {
    return elements.empty() ? "" : elements.back().first;
}

bool Stack::empty() const {
    return elements.empty();
}

string Stack::toString() const {
    stringstream ss;
    for (const auto& e : elements) {
        ss << e.first << e.second << " ";
    }
    return ss.str();
}
