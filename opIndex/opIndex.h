#ifndef _OPINDEX_H
#define _OPINDEX_H
#include <ctime>
#include "data_structure.h"
#include "generator.h"
#include "printer.h"
#include "chrono_time.h"
#include <string>
#include <cstring>

const int MAX_ATTS = 101;
const int MAX_SUBS = 2000001;
const int SEGMENTS = 32;
const int MAX_SIGNATURE = 61;

class opIndex {
    vector<ConElement> data[MAX_ATTS][3][SEGMENTS][MAX_SIGNATURE];
    bool sig[MAX_ATTS][3][SEGMENTS][MAX_SIGNATURE];
    int counter[MAX_SUBS];
    bool isPivot[MAX_ATTS];
    int fre[MAX_ATTS];

    void initCounter(vector<Sub> &subList);

    void initCounter(vector<IntervalSub> &subList);

    int getMinFre(Sub x);

    int getMinFre(IntervalSub x);

    int signatureHash1(int att, int val);       // for == operation
    int signatureHash2(int att);                // for <= and >= operation
public:
    opIndex()
    {
        memset(isPivot, 0, sizeof(isPivot));
        memset(sig, 0, sizeof(sig));
    }

    void calcFrequency(vector<Sub> &subList);

    void calcFrequency(vector<IntervalSub> &subList);

    void insert(Sub x);

    void insert(IntervalSub x);

    void match(Pub x, int &matchSubs, vector<Sub> &subList);

    void match(Pub x, int &matchSubs, vector<IntervalSub> &subList);
};

#endif
