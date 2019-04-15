#ifndef REIN_REIN_H
#define REIN_REIN_H
#include<vector>
#include <cstring>
#include "generator.h"
#include "chrono_time.h"
#include "util.h"
#include "data_structure.h"

const int MAX_SUBS = 2000001;
const int MAX_ATTS = 3000;
const int MAX_BUCKS = 2000;

class Rein{
    int valDom, buckStep, bucks;
    vector<Combo> data[MAX_ATTS][2][MAX_BUCKS];    // 0:left parenthesis, 1:right parenthesis
    bool bits[MAX_SUBS];
    bool skipped[MAX_ATTS];
    attAndCount attsCounts[MAX_ATTS];
public:
    Rein(int _valDom){ valDom = _valDom; buckStep = (valDom - 1) / MAX_BUCKS + 1; bucks = (valDom - 1) / buckStep + 1; }
    void insert(IntervalSub sub);
    int64_t match(const Pub &pub, int &matchSubs, const vector<IntervalSub> &subList);
    void select_skipped_atts(const vector<IntervalSub> &subList, int atts, double falsePositive);
    int64_t approx_match(const Pub &pub, int &matchSubs, const vector<IntervalSub> &subList);
};

#endif //REIN_REIN_H
