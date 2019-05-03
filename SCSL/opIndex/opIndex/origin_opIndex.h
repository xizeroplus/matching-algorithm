#ifndef _origin_opIndex_H
#define _origin_opIndex_H
#include <ctime>
#include "data_structure.h"
#include "generator.h"
#include "printer.h"
#include "chrono_time.h"
#include <string>
#include <cstring>
#include <list>
#include <set>
#include <tuple>
const int MAX_ATTS_origin = 101;
const int MAX_SUBS_origin = 2000001;
const int SEGMENTS_origin = 100;
const int MAX_SIGNATURE_origin = 2000;

class origin_opIndex {
    vector<originConElement> data[MAX_ATTS_origin][2][SEGMENTS_origin];
    bool sig[MAX_ATTS_origin][2][SEGMENTS_origin][MAX_SIGNATURE_origin];
    int counter[MAX_SUBS_origin];
    bool isPivot[MAX_ATTS_origin];
    int fre[MAX_ATTS_origin];
    int act_seg;
    int act_sig;

    void initCounter(vector<IntervalSub> &subList);
    void initCounter(vector<Pub> &pubList);

    //int getMinFre(Sub x);

    int getMinFre(IntervalSub x);

    int signatureHash1(int att, int val);       // for == operation
    int signatureHash2(int att);                // for <= and >= operation

public:
    origin_opIndex(int segs, int sigs)
    {
        act_seg = SEGMENTS_origin;
        act_sig = MAX_SIGNATURE_origin;
        memset(isPivot, 0, sizeof(isPivot));
        memset(sig, 0, sizeof(sig));
    }


    void calcFrequency(vector<IntervalSub> &subList);


    void insert(IntervalSub &x);

    void match(Pub &pub, int &matchSubs, vector<IntervalSub> &subList, vector<double> &matchDetailList, vector<int> &matchDetailSub);

};

#endif
