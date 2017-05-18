#ifndef SIENA_H
#define SIENA_H
#include "util.h"
using namespace std;

const int MAX_SUBS = 200000;
const int MAX_ATTS = 2000;

class Siena{
    int counter[MAX_SUBS];
    vector<IntervalCombo> data[MAX_ATTS];
public:
    Siena(){}
    void insert(IntervalSub sub);
    void match(const Pub &pub, int &matchSubs, const vector<IntervalSub> &subList);
};

#endif
