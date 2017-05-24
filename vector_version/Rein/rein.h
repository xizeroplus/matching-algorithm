#ifndef _REIN_H
#define _REIN_H
#include "util.h"

const int MAX_SUBS = 2000001;
const int MAX_ATTS = 101;
const int MAX_BUCKS = 500;

class Rein{
    int valDom, buckStep, bucks;
    vector<Combo> data[MAX_ATTS][2][MAX_BUCKS];    // 0:left parenthesis, 1:right parenthesis
public:
    Rein(int _valDom){ valDom = _valDom; buckStep = (valDom - 1) / MAX_BUCKS + 1; bucks = (valDom - 1) / buckStep + 1; }
    void insert(IntervalSub sub);
    void match(const Pub &pub, int &matchSubs, const vector<IntervalSub> &subList);
};

#endif //_REIN_H
