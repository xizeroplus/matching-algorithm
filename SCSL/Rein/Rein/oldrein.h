#ifndef OLD_REIN_H
#define OLD_REIN_H
#include<vector>
#include <cstring>
#include "generator.h"
#include "chrono_time.h"
#include "util.h"
#include "data_structure.h"

const int OLD_MAX_SUBS = 3000001;
const int OLD_MAX_ATTS = 21;
const int OLD_MAX_BUCKS = 201;

class oldRein{
    int valDom, buckStep, bucks;
    vector<Combo> data[OLD_MAX_ATTS][2][OLD_MAX_BUCKS];    // 0:left parenthesis, 1:right parenthesis
public:
    oldRein(int _valDom, int buck_num){ valDom = _valDom; buckStep = (valDom - 1) / buck_num + 1; bucks = (valDom - 1) / buckStep + 1; }
    void insert(Sub &sub);
    void insert(IntervalSub &sub);
    void match(const Pub &pub, int &matchSubs, const vector<IntervalSub> &subList, vector<double> &matchDetailPub);
};

#endif
