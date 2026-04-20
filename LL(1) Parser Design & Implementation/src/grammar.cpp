#include "grammar.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <stdexcept>

using namespace std;

bool Grammar::isNonTerminal(const string& sym) const {
    return productions.count(sym) > 0 ||
           find(nonTerminals.begin(), nonTerminals.end(), sym) != nonTerminals.end();
}

bool Grammar::isTerminal(const string& sym) const {
    return terminals.count(sym) > 0;
}

bool Grammar::isEpsilon(const string& sym) const {
    return sym == "epsilon" || sym == "@";
}

vector<string> Grammar::tokenizeLine(const string& line) const {
    vector<string> tokens;
    istringstream iss(line);
    string tok;
    while (iss >> tok) tokens.push_back(tok);
    return tokens;
}

//  Load grammar from file
bool Grammar::loadFromFile(const string& filename) {
    ifstream fin(filename);
    if (!fin.is_open()) {
        cerr << "[ERROR] Cannot open grammar file: " << filename << "\n";
        return false;
    }

    string line;
    int lineNo = 0;

    while (getline(fin, line)) {
        ++lineNo;

        // Strip comments (# …) and leading/trailing whitespace
        auto hashPos = line.find('#');
        if (hashPos != string::npos) line = line.substr(0, hashPos);
        // trim
        size_t s = line.find_first_not_of(" \t\r\n");
        if (s == string::npos) continue;          // blank line
        size_t e = line.find_last_not_of(" \t\r\n");
        line = line.substr(s, e - s + 1);
        if (line.empty()) continue;

        // Split on "->"
        auto arrowPos = line.find("->");
        if (arrowPos == string::npos) {
            cerr << "[WARNING] Line " << lineNo
                      << ": missing '->',  skipping: " << line << "\n";
            continue;
        }

        // LHS
        string lhsRaw = line.substr(0, arrowPos);
        // trim lhsRaw
        size_t ls = lhsRaw.find_first_not_of(" \t");
        size_t le = lhsRaw.find_last_not_of(" \t");
        if (ls == string::npos) {
            cerr << "[WARNING] Line " << lineNo << ": empty LHS, skipping.\n";
            continue;
        }
        string lhs = lhsRaw.substr(ls, le - ls + 1);

        // Validate: non-terminal must start with uppercase and be multi-char
        if (!isupper(lhs[0])) {
            cerr << "[ERROR] Line " << lineNo
                      << ": Non-terminal '" << lhs
                      << "' must start with an uppercase letter.\n";
            return false;
        }
        if (lhs.size() == 1) {
            cerr << "[ERROR] Line " << lineNo
                      << ": Single-character non-terminals ('" << lhs
                      << "') are NOT allowed. Use multi-character names (e.g., Expr).\n";
            return false;
        }

        // Register non-terminal (preserving order)
        if (find(nonTerminals.begin(), nonTerminals.end(), lhs) == nonTerminals.end()) {
            nonTerminals.push_back(lhs);
        }
        if (startSymbol.empty()) startSymbol = lhs;

        // RHS: split on '|'
        string rhsRaw = line.substr(arrowPos + 2);
        istringstream rhsStream(rhsRaw);
        string alt;
        while (getline(rhsStream, alt, '|')) {
            // trim alt
            size_t as = alt.find_first_not_of(" \t");
            size_t ae = alt.find_last_not_of(" \t");
            if (as == string::npos) {
                cerr << "[WARNING] Line " << lineNo
                          << ": empty alternative in production for '" << lhs << "', skipping.\n";
                continue;
            }
            alt = alt.substr(as, ae - as + 1);

            vector<string> symbols = tokenizeLine(alt);

            // Normalise epsilon
            for (auto& sym : symbols) {
                if (sym == "@") sym = "epsilon";
            }

            // Validate: production should be either epsilon alone or non-epsilon symbols
            if (symbols.size() > 1) {
                for (const auto& sym : symbols) {
                    if (isEpsilon(sym)) {
                        cerr << "[ERROR] Line " << lineNo
                                  << ": 'epsilon' cannot appear with other symbols in a production.\n";
                        return false;
                    }
                }
            }

            // Collect terminals (any symbol that is not a known non-terminal and not epsilon)
          
            productions[lhs].push_back(symbols);
        }
    }

    fin.close();

    if (nonTerminals.empty()) {
        cerr << "[ERROR] Grammar file is empty or has no valid productions.\n";
        return false;
    }

    // Collect terminals: symbols that are not non-terminals and not epsilon
    for (auto& [nt, alts] : productions) {
        for (auto& alt : alts) {
            for (auto& sym : alt) {
                if (!isEpsilon(sym) &&
                    find(nonTerminals.begin(), nonTerminals.end(), sym) == nonTerminals.end()) {
                    terminals.insert(sym);
                }
            }
        }
    }

    return true;
}


//  Generate a fresh non-terminal name not already in the grammar


string Grammar::freshNT(const string& base) const {
    string candidate = base + "Prime";
    int suffix = 2;
    while (find(nonTerminals.begin(), nonTerminals.end(), candidate) != nonTerminals.end()) {
        candidate = base + "Prime" + to_string(suffix++);
    }
    return candidate;
}

//  LEFT FACTORING
void Grammar::factorNonTerminal(const string& nt) {
    bool changed = true;
    while (changed) {
        changed = false;
        auto& alts = productions[nt];

        // Group alternatives by their first symbol
        map<string, vector<int>> firstSymGroups;
        for (int i = 0; i < (int)alts.size(); ++i) {
            string key = alts[i].empty() ? "epsilon" : alts[i][0];
            firstSymGroups[key].push_back(i);
        }

        for (auto& [firstSym, indices] : firstSymGroups) {
            if (indices.size() < 2) continue;   // no common prefix here

            // Find the longest common prefix among alternatives with same first symbol
            // Start with the first alternative as reference
            vector<string> prefix = alts[indices[0]];
            for (size_t k = 1; k < indices.size(); ++k) {
                const auto& other = alts[indices[k]];
                size_t len = min(prefix.size(), other.size());
                size_t match = 0;
                while (match < len && prefix[match] == other[match]) ++match;
                prefix.resize(match);
                if (prefix.empty()) break;
            }
            if (prefix.empty()) continue;

            // We have a common prefix of length prefix.size()
            // Create new NT
            string newNT = freshNT(nt);
            nonTerminals.push_back(newNT);

            // Build new alternatives for newNT (the suffixes)
            vector<vector<string>> newAlts;
            for (int idx : indices) {
                vector<string> suffix(alts[idx].begin() + prefix.size(), alts[idx].end());
                if (suffix.empty()) suffix.push_back("epsilon");
                newAlts.push_back(suffix);
            }
            productions[newNT] = newAlts;

            // Replace the grouped alternatives in nt with one factored alternative
            // Build the factored production:  prefix newNT
            vector<string> factoredAlt = prefix;
            factoredAlt.push_back(newNT);

            // Remove old alternatives (in reverse order to keep indices valid)
            vector<int> sortedIdx = indices;
            sort(sortedIdx.rbegin(), sortedIdx.rend());
            for (int idx : sortedIdx) alts.erase(alts.begin() + idx);

            // Add the factored alternative
            alts.push_back(factoredAlt);

            changed = true;
            break;  // restart loop for this NT
        }
    }
}

void Grammar::applyLeftFactoring() {
    // We need to iterate over a snapshot of NT names because factorNonTerminal
    // may add new ones; newly added ones also need to be factored.
    size_t i = 0;
    while (i < nonTerminals.size()) {
        factorNonTerminal(nonTerminals[i]);
        ++i;
    }

    // Refresh terminal set (new NTs may have been added)
    terminals.clear();
    for (auto& [nt, alts] : productions) {
        for (auto& alt : alts) {
            for (auto& sym : alt) {
                if (!isEpsilon(sym) &&
                    find(nonTerminals.begin(), nonTerminals.end(), sym) == nonTerminals.end()) {
                    terminals.insert(sym);
                }
            }
        }
    }
}

//  LEFT RECURSION REMOVAL
void Grammar::substituteProductions(const string& ai, const string& aj) {
    auto& aiAlts = productions[ai];
    auto& ajAlts = productions[aj];

    vector<vector<string>> newAlts;
    bool substituted = false;

    for (auto& alt : aiAlts) {
        if (!alt.empty() && alt[0] == aj) {
            // Replace with each production of Aj followed by the rest of alt
            substituted = true;
            vector<string> suffix(alt.begin() + 1, alt.end());
            for (auto& ajAlt : ajAlts) {
                vector<string> newAlt;
                if (!(ajAlt.size() == 1 && isEpsilon(ajAlt[0]))) {
                    newAlt = ajAlt;
                }
                newAlt.insert(newAlt.end(), suffix.begin(), suffix.end());
                if (newAlt.empty()) newAlt.push_back("epsilon");
                newAlts.push_back(newAlt);
            }
        } else {
            newAlts.push_back(alt);
        }
    }

    if (substituted) aiAlts = newAlts;
}

// Eliminate direct left recursion from non-terminal nt
void Grammar::eliminateDirectLR(const string& nt) {
    auto& alts = productions[nt];

    // Separate recursive (A -> A α) and non-recursive (A -> β) alternatives
    vector<vector<string>> recursive, nonRecursive;
    for (auto& alt : alts) {
        if (!alt.empty() && alt[0] == nt) {
            // A -> A α  =>  α is the tail
            vector<string> tail(alt.begin() + 1, alt.end());
            if (tail.empty()) tail.push_back("epsilon");
            recursive.push_back(tail);
        } else {
            nonRecursive.push_back(alt);
        }
    }

    if (recursive.empty()) return;  // no direct left recursion

    // Create A'
    string newNT = freshNT(nt);
    nonTerminals.push_back(newNT);

    // A  ->  β1 A' | β2 A' | …
    vector<vector<string>> newAlts;
    for (auto& beta : nonRecursive) {
        vector<string> prod = beta;
        // If beta is epsilon, the production is just A'
        if (prod.size() == 1 && isEpsilon(prod[0])) {
            prod = { newNT };
        } else {
            prod.push_back(newNT);
        }
        newAlts.push_back(prod);
    }
    // Edge case: if there are no non-recursive alternatives
    if (newAlts.empty()) {
        newAlts.push_back({ newNT });
    }
    productions[nt] = newAlts;

    // A' ->  α1 A' | α2 A' | … | epsilon
    vector<vector<string>> primAlts;
    for (auto& alpha : recursive) {
        vector<string> prod = alpha;
        if (prod.size() == 1 && isEpsilon(prod[0])) {
            prod = { newNT };
        } else {
            prod.push_back(newNT);
        }
        primAlts.push_back(prod);
    }
    primAlts.push_back({ "epsilon" });
    productions[newNT] = primAlts;
}

void Grammar::removeLeftRecursion() {
    // Work on a snapshot of the current NT list (order matters!)
    vector<string> ntOrder = nonTerminals;

    for (size_t i = 0; i < ntOrder.size(); ++i) {
        const string& ai = ntOrder[i];

        // For each j < i: substitute Aj into Ai
        for (size_t j = 0; j < i; ++j) {
            const string& aj = ntOrder[j];
            substituteProductions(ai, aj);
        }

        // Eliminate any direct left recursion in Ai
        eliminateDirectLR(ai);
    }

    // Refresh terminal set
    terminals.clear();
    for (auto& [nt, alts] : productions) {
        for (auto& alt : alts) {
            for (auto& sym : alt) {
                if (!isEpsilon(sym) &&
                    find(nonTerminals.begin(), nonTerminals.end(), sym) == nonTerminals.end()) {
                    terminals.insert(sym);
                }
            }
        }
    }
}

//  Display helpers

static string productionToString(const string& lhs,
                                      const vector<vector<string>>& alts) {
    string result = lhs + " -> ";
    for (size_t i = 0; i < alts.size(); ++i) {
        if (i > 0) result += " | ";
        for (size_t j = 0; j < alts[i].size(); ++j) {
            if (j > 0) result += " ";
            result += alts[i][j];
        }
    }
    return result;
}

void Grammar::display(const string& title) const {
    if (!title.empty()) {
        cout << "\n" << string(60, '=') << "\n";
        cout << "  " << title << "\n";
        cout << string(60, '=') << "\n";
    }
    cout << "  Start Symbol : " << startSymbol << "\n";
    cout << "  Non-Terminals: ";
    for (size_t i = 0; i < nonTerminals.size(); ++i) {
        if (i) cout << ", ";
        cout << nonTerminals[i];
    }
    cout << "\n  Terminals    : ";
    bool first = true;
    for (auto& t : terminals) {
        if (!first) cout << ", ";
        cout << t;
        first = false;
    }
    cout << "\n\n  Productions:\n";
    for (auto& nt : nonTerminals) {
        if (productions.count(nt)) {
            cout << "    " << productionToString(nt, productions.at(nt)) << "\n";
        }
    }
    cout << string(60, '-') << "\n";
}

void Grammar::writeToFile(const string& filename, const string& title) const {
    ofstream fout(filename, ios::app);   // append so multiple sections go in one file
    if (!fout.is_open()) {
        cerr << "[ERROR] Cannot open output file: " << filename << "\n";
        return;
    }
    if (!title.empty()) {
        fout << "\n" << string(60, '=') << "\n";
        fout << "  " << title << "\n";
        fout << string(60, '=') << "\n";
    }
    fout << "  Start Symbol : " << startSymbol << "\n";
    fout << "  Non-Terminals: ";
    for (size_t i = 0; i < nonTerminals.size(); ++i) {
        if (i) fout << ", ";
        fout << nonTerminals[i];
    }
    fout << "\n  Terminals    : ";
    bool first = true;
    for (auto& t : terminals) {
        if (!first) fout << ", ";
        fout << t;
        first = false;
    }
    fout << "\n\n  Productions:\n";
    for (auto& nt : nonTerminals) {
        if (productions.count(nt)) {
            fout << "    " << productionToString(nt, productions.at(nt)) << "\n";
        }
    }
    fout << string(60, '-') << "\n";
    fout.close();
}
