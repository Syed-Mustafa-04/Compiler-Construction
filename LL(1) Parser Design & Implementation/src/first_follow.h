#ifndef FIRST_FOLLOW_H
#define FIRST_FOLLOW_H

#include "grammar.h"
#include <map>
#include <set>
#include <string>
#include <vector>

using namespace std;

class FirstFollow {
public:
   
    map<string, set<string>> firstSets;

    map<string, set<string>> followSets;


    explicit FirstFollow(const Grammar& g);

    //Computation entry points
    void computeFirst();
    void computeFollow();

    // ── Query helpers ─────────────────────────────────────────
    // FIRST of an arbitrary sequence of symbols  α = [s0, s1, …]
    set<string> firstOfSequence(
        const vector<string>& seq) const;
    void display() const;
    void writeToFile(const string& filename) const;

private:
    const Grammar& gram;   
    bool addFirstOf(const string& sym,
                    set<string>& dest) const;
};

#endif 
