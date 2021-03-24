#ifndef _GENERATOR_H
#define _GENERATOR_H
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include "data_structure.h"
using namespace std;

const int HOT_NUMBERS = 2;
const int GEN_MAX_ATTS = 21;

class intervalGenerator {
	int subs, pubs, atts, cons, m, valDom;
	double width;
	bool hot;
	int hotStart[GEN_MAX_ATTS][HOT_NUMBERS];
	double hotSplit[GEN_MAX_ATTS][HOT_NUMBERS], randnumtmp[HOT_NUMBERS];
	IntervalSub GenOneSub(int id);
	IntervalSub GenOneHotSub(int id);
	Pub GenOnePub(int id);
	Pub GenOneHotPub(int id);
	void GenHot();
	int GetHot(int att, int num);

public:
    vector<IntervalSub> subList;
    vector<Pub> pubList;
    

    intervalGenerator(int subs, int pubs, int atts, int cons, int m, int valDom, double width) :
            subs(subs), pubs(pubs), atts(atts), cons(cons), m(m), valDom(valDom), width(width), hot(false)
    {
		 srand(time(NULL));
	}
	void GenSubList();
	void ReadSubList();
	void GenHotSubList();
	void GenPubList();
	void GenHotPubList();
	void ReadPubList();
};

#endif
