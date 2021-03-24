#ifndef _ENMATP_H
#define _ENMATP_H

#include <thread>
#include <functional>
#include "rein.h"
#include "tama.h"

class Enmatp{
	int valDom;
    double splitpoint, maxwidth, minwidth;
    bool randominit;
    int64_t totalt1, totalt2;
	void reinmatch(const Pub &pub, int &matchSubs, int64_t &t);
	void tamamatch(const Pub &pub, const vector<IntervalSub> &subList, int &matchSubs, int64_t &t);
public:
    Enmatp(int valDom, int buck_num, int atts, int level, bool randominit = false):valDom(valDom), rein(valDom, buck_num), tama(atts, valDom, level),
    randominit(randominit), splitpoint(0.5), totalt1(0), totalt2(0), maxwidth(0.9), minwidth(0.1){
	}
	Rein rein;
	Tama tama;
    void insert(const IntervalSub &sub);
    void match(const Pub &pub, const vector<IntervalSub> &subList, int &matchSubs1, int &matchSubs2, int64_t &t1, int64_t &t2);
    double adjustsplit();
};

#endif
