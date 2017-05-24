#ifndef _OPINDEX_H
#define _OPINDEX_H
#include "util.h"

const int MAX_ATTS = 101;
const int SEGMENTS = 32;
const int MAX_SIGNATURE = 61;

class opIndex{
    vector<ConElement> data[MAX_ATTS][3][SEGMENTS][MAX_SIGNATURE];
    bool sig[MAX_ATTS][3][SEGMENTS][MAX_SIGNATURE];
    vector<int> counter;
    bool isPivot[MAX_ATTS];
    int fre[MAX_ATTS];
    void initCounter(vector<IntervalSub> &subList);
    int getMinFre(IntervalSub x);
    int signatureHash1(int att, int val);       // for == operation
    int signatureHash2(int att);                // for <= and >= operation
public:
    opIndex(){ memset(isPivot, 0, sizeof(isPivot)); memset(sig, 0, sizeof(sig)); }
    void calcFrequency(vector<IntervalSub> &subList);
    void insert(IntervalSub x);
    void match(Pub x, int &matchSubs, vector<IntervalSub> &subList);
};

#endif
