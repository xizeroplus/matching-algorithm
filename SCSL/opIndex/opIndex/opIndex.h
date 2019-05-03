#ifndef _OPINDEX_H
#define _OPINDEX_H
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

const int MAX_ATTS = 21;
const int MAX_SUBS = 2000001;
const int SEGMENTS = 100;
const int MAX_SIGNATURE = 2000;
class opIndex {
    vector<ConElement> data[MAX_ATTS][2][SEGMENTS];
    bool sig[MAX_ATTS][2][SEGMENTS][MAX_SIGNATURE];
    int counter[MAX_SUBS],countlist[MAX_ATTS],order[MAX_ATTS],totalcount[MAX_ATTS];
    bool isPivot[MAX_ATTS];
    int fre[MAX_ATTS];
    int act_seg;
    int act_sig;

    void initCounter(vector<Pub> &pubList);

    void initCounter(vector<IntervalSub> &subList);

    //int getMinFre(Sub x);

    int getMinFre(IntervalSub x);

    int signatureHash1(int att, int val);       // for == operation
    int signatureHash2(int att);                // for <= and >= operation

public:

    opIndex(int segs, int sigs)
    {
        act_seg = SEGMENTS;
        act_sig = MAX_SIGNATURE;
        memset(isPivot, 0, sizeof(isPivot));
        memset(sig, 0, sizeof(sig));
		memset(countlist, 0, sizeof(countlist));
		memset(totalcount, 0, sizeof(totalcount));
		for(int i=0;i<MAX_ATTS;++i) order[i]=i;
    }

    void calcFrequency(vector<IntervalSub> &subList);



    void insert(IntervalSub &x);

    


    void match(Pub &pub, int &matchSubs, vector<IntervalSub>& subList, vector<double> &matchDetailPub, vector<int> &matchDetailSub);
	
	void change();


};

#endif
