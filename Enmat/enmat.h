#ifndef _ENMAT_H
#define _ENMAT_H

#include "rein.h"
#include "tama.h"

class Enmat{
	int valDom;
    double splitpoint;
    bool randominit;
public:
    Enmat(int valDom, int buck_num, int atts, int level, bool randominit = false):valDom(valDom), rein(valDom, buck_num), tama(atts, valDom, level),
    randominit(randominit), splitpoint(0.5){
	}
	Rein rein;
	Tama tama;
    void insert(const IntervalSub &sub);
    void match(const Pub &pub, const vector<IntervalSub> &subList, int &matchSubs1, int &matchSubs2, int64_t &t1, int64_t &t2);
};

#endif
