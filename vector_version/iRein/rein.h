#ifndef REIN_REIN_H
#define REIN_REIN_H
#include<vector>
#include <cstring>
#include "generator.h"
#include "chrono_time.h"
#include "util.h"
#include "data_structure.h"

const int MAX_ATTS = 2000;
const int MAX_BUCKS = 500;

class Rein{
    int valDom, buckStep, bucks;
    vector<Combo> data[MAX_ATTS][2][MAX_BUCKS];    // 0:left parenthesis, 1:right parenthesis
public:
    Rein(int _valDom){ valDom = _valDom; buckStep = (valDom - 1) / MAX_BUCKS + 1; bucks = (valDom - 1) / buckStep + 1; }
    void insert(IntervalSub sub);
    void match(const Pub &pub, int &matchSubs, const vector<IntervalSub> &subList, int x);
};

#endif //REIN_REIN_H
