#ifndef _REIN_H
#define _REIN_H
#define _USE_MATH_DEFINES
#include<vector>
#include <cstring>
#include "generator.h"
#include "chrono_time.h"
#include "util.h"
#include "data_structure.h"
#include <algorithm>
#include <cmath>

const int MAX_SUBS = 3000001;
const int MAX_ATTS = 21;
const int MAX_BUCKS = 100;
const int MAX_CONS = 21;

class Rein{
    int valDom, Dom, subnum;
    vector<kCombo> data[MAX_CONS][MAX_ATTS][2][MAX_BUCKS];    // 0:left parenthesis, 1:right parenthesis
    vector<int> LvSize[MAX_CONS];
	int countlist[MAX_SUBS];
	int originlevel[MAX_SUBS];
    int LvBuck[MAX_CONS];
    int LvBuckStep[MAX_CONS];
	static bool comp(WeightPair &i,WeightPair &j);
	int subtolevel[MAX_SUBS]; //sub.id => level
	vector<changeaction> changelist[MAX_CONS];

	bool firstchange;
	double adjustcost;
	double limitscale;
	int newlevel;
public:
    Rein(int _valDom, int k, int buck_num, double limitscale,int newlevel):valDom(_valDom),Dom(k),limitscale(limitscale),newlevel(newlevel)
	{
		subnum = 0;
        for (int i=0; i<newlevel; ++i)
        {
            LvBuck[i] = int(buck_num * sqrt(i+1) / newlevel) + 1;
            LvBuckStep[i] = (valDom - 1) / LvBuck[i] + 1;
            LvBuck[i] = (valDom - 1) / LvBuckStep[i] + 1;
            //cout << LvBuck[i] << endl;
        }

		memset(countlist, 0, sizeof(countlist));
		firstchange = true;
    }
    //void insert(Sub &sub);
	void insert(IntervalSub &sub, int64_t &origintime);
    void match(const Pub &pub, int &matchSubs, const vector<IntervalSub> &subList, vector<double> &matchDetailPub);
	int change(const vector<IntervalSub> &subList,int cstep, double matchingtime, string &windowcontent); //return numbers of subs changed by function
	void check();
};

#endif
