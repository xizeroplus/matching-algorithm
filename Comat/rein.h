#ifndef _REIN_H
#define _REIN_H
#include <vector>
#include "generator.h"
#include "chrono_time.h"
#include "util.h"
#include "data_structure.h"

const int MAX_ATTS = 21;
const int MAX_BUCKS = 1001;

class Rein{
    int valDom, buckStep, bucks, subsize;
    vector<Combo> data[MAX_ATTS][2][MAX_BUCKS];    // 0:left parenthesis, 1:right parenthesis
public:
    Rein(int valDom, int buck_num):valDom(valDom),subsize(0){
		buckStep = (valDom - 1) / buck_num + 1;
		bucks = (valDom - 1) / buckStep + 1;
		}
    void insert(const IntervalSub &sub);
    void match(const Pub &pub, int &matchSubs);
};

#endif
