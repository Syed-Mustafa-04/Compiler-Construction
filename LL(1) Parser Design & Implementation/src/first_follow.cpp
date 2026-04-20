#include "first_follow.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace std;


FirstFollow::FirstFollow(const Grammar& g) : gram(g) {
    // Initialise empty sets for every non-terminal
    for (const auto& nt : gram.nonTerminals) {
        firstSets[nt];   // default-constructs an empty set
        followSets[nt];
    }
}

bool FirstFollow::addFirstOf(const string& sym,
                              set<string>& dest) const {
    if (gram.isEpsilon(sym)) {
        // epsilon itself: epsilon is derivable trivially
        return true;
    }
    if (gram.isTerminal(sym)) {
        dest.insert(sym);
        return false;   // a terminal cannot derive epsilon
    }
    // sym is a non-terminal
    auto it = firstSets.find(sym);
    if (it == firstSets.end()) {
        // Unknown symbol – treat as terminal (shouldn't happen after
        // grammar validation, but be safe)
        dest.insert(sym);
        return false;
    }
    bool hasEps = false;
    for (const auto& tok : it->second) {
        if (tok == "epsilon") {
            hasEps = true;
        } else {
            dest.insert(tok);
        }
    }
    return hasEps;
}


void FirstFollow::computeFirst() {

    bool changed = true;
    while (changed) {
        changed = false;

        for (const auto& nt : gram.nonTerminals) {
            auto it = gram.productions.find(nt);
            if (it == gram.productions.end()) continue;

            for (const auto& alt : it->second) {
                // alt is one RHS alternative: a sequence of symbols
                if (alt.empty()) continue;

                if (alt.size() == 1 && gram.isEpsilon(alt[0])) {
                    // X -> epsilon
                    if (firstSets[nt].insert("epsilon").second) changed = true;
                    continue;
                }

                // X -> Y1 Y2 … Yn
                bool allCanDeriveEps = true;
                for (const auto& sym : alt) {
                    if (gram.isEpsilon(sym)) {
                        // Epsilon mid-sequence 
                        break;
                    }

                    set<string> toAdd;
                    bool symHasEps = false;

                    if (gram.isTerminal(sym)) {
                        toAdd.insert(sym);
                        symHasEps = false;
                    } else {
                        // Non-terminal: copy its FIRST minus epsilon
                        auto& fs = firstSets[sym];
                        for (const auto& t : fs) {
                            if (t == "epsilon") symHasEps = true;
                            else                toAdd.insert(t);
                        }
                    }

                    for (const auto& t : toAdd) {
                        if (firstSets[nt].insert(t).second) changed = true;
                    }

                    if (!symHasEps) {
                        allCanDeriveEps = false;
                        break;
                    }
                }

                if (allCanDeriveEps) {
                    if (firstSets[nt].insert("epsilon").second) changed = true;
                }
            }
        }
    }
}


//  FIRST of an arbitrary sequence  α = [s0, s1, …]
set<string> FirstFollow::firstOfSequence(
        const vector<string>& seq) const {

    set<string> result;

    if (seq.empty() ||
        (seq.size() == 1 && gram.isEpsilon(seq[0]))) {
        result.insert("epsilon");
        return result;
    }

    bool allDeriveEps = true;
    for (const auto& sym : seq) {
        if (gram.isEpsilon(sym)) {
            // epsilon token mid-sequence
            continue;
        }

        bool symHasEps = false;
        if (gram.isTerminal(sym)) {
            result.insert(sym);
            symHasEps = false;
        } else {
            auto it = firstSets.find(sym);
            if (it != firstSets.end()) {
                for (const auto& t : it->second) {
                    if (t == "epsilon") symHasEps = true;
                    else                result.insert(t);
                }
            } else {
                // Unknown – treat as terminal
                result.insert(sym);
                symHasEps = false;
            }
        }

        if (!symHasEps) {
            allDeriveEps = false;
            break;
        }
    }

    if (allDeriveEps) result.insert("epsilon");
    return result;
}

void FirstFollow::computeFollow() {
    // Rule 1
    followSets[gram.startSymbol].insert("$");

    bool changed = true;
    while (changed) {
        changed = false;

        for (const auto& nt : gram.nonTerminals) {
            auto prodIt = gram.productions.find(nt);
            if (prodIt == gram.productions.end()) continue;

            for (const auto& alt : prodIt->second) {
                // For each symbol position in the RHS
                for (size_t i = 0; i < alt.size(); ++i) {
                    const string& B = alt[i];

                    // We only compute FOLLOW for non-terminals
                    if (!gram.isNonTerminal(B) || gram.isEpsilon(B)) continue;

                    // β = symbols after B in this alternative
                    vector<string> beta(alt.begin() + i + 1, alt.end());

                    // FIRST(β)
                    set<string> firstBeta = firstOfSequence(beta);

                    // Rule 2: add FIRST(β) \ {ε} to FOLLOW(B)
                    for (const auto& t : firstBeta) {
                        if (t != "epsilon") {
                            if (followSets[B].insert(t).second) changed = true;
                        }
                    }

                    // Rule 3: if ε ∈ FIRST(β), add FOLLOW(A) to FOLLOW(B)
                    if (firstBeta.count("epsilon")) {
                        for (const auto& t : followSets[nt]) {
                            if (followSets[B].insert(t).second) changed = true;
                        }
                    }
                }
            }
        }
    }
}

static void printSetLine(ostream& out,
                         const string& nt,
                         const set<string>& s,
                         int colWidth) {
    out << "    " << left << setw(colWidth) << nt << " : { ";
    bool first = true;
    for (const auto& t : s) {
        if (!first) out << ", ";
        out << t;
        first = false;
    }
    out << " }\n";
}

static int maxNTWidth(const vector<string>& nts) {
    int w = 0;
    for (const auto& nt : nts) w = max(w, (int)nt.size());
    return w + 2;
}

void FirstFollow::display() const {
    int w = maxNTWidth(gram.nonTerminals);

    cout << "\n" << string(60, '=') << "\n";
    cout << "  Task 1.4  –  FIRST Sets\n";
    cout << string(60, '=') << "\n";
    for (const auto& nt : gram.nonTerminals) {
        auto it = firstSets.find(nt);
        if (it != firstSets.end())
            printSetLine(cout, nt, it->second, w);
    }
    cout << string(60, '-') << "\n";

    cout << "\n" << string(60, '=') << "\n";
    cout << "  Task 1.5  –  FOLLOW Sets\n";
    cout << string(60, '=') << "\n";
    for (const auto& nt : gram.nonTerminals) {
        auto it = followSets.find(nt);
        if (it != followSets.end())
            printSetLine(cout, nt, it->second, w);
    }
    cout << string(60, '-') << "\n";
}

void FirstFollow::writeToFile(const string& filename) const {
    ofstream fout(filename, ios::app);
    if (!fout.is_open()) {
        cerr << "[ERROR] Cannot open output file: " << filename << "\n";
        return;
    }

    int w = maxNTWidth(gram.nonTerminals);

    fout << "\n" << string(60, '=') << "\n";
    fout << "  Task 1.4  –  FIRST Sets\n";
    fout << string(60, '=') << "\n";
    for (const auto& nt : gram.nonTerminals) {
        auto it = firstSets.find(nt);
        if (it != firstSets.end())
            printSetLine(fout, nt, it->second, w);
    }
    fout << string(60, '-') << "\n";

    fout << "\n" << string(60, '=') << "\n";
    fout << "  Task 1.5  –  FOLLOW Sets\n";
    fout << string(60, '=') << "\n";
    for (const auto& nt : gram.nonTerminals) {
        auto it = followSets.find(nt);
        if (it != followSets.end())
            printSetLine(fout, nt, it->second, w);
    }
    fout << string(60, '-') << "\n";

    fout.close();
}
