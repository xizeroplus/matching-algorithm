#ifndef REIN_REIN_H
#define REIN_REIN_H
#include<vector>
#include <cstring>
#include "generator.h"
#include "chrono_time.h"
#include "util.h"
#include "data_structure.h"

const int MAX_SUBS = 200000;
const int MAX_ATTS = 1001;
const int MAX_BUCKS = 500;

class Rein{
    int valDom, buckStep, bucks;
    vector<Combo> data[MAX_ATTS][2][MAX_BUCKS];    // 0:left parenthesis, 1:right parenthesis
    bool bits[MAX_SUBS];
public:
    Rein(int _valDom){ valDom = _valDom; buckStep = (valDom - 1) / MAX_BUCKS + 1; bucks = (valDom - 1) / buckStep + 1; }
    void insert(Sub sub);
    void insert(IntervalSub sub);
    void match(const Pub &pub, int &matchSubs, const vector<Sub> &subList);
    //void match(const Pub &pub, int &matchSubs, const vector<IntervalSub> &subList);
    int64_t match(const Pub &pub, int &matchSubs, const vector<IntervalSub> &subList, int x);
};

#endif //REIN_REIN_H
