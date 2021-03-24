#include "enmatp.h"

static inline double random1()
{
    return (rand() / (RAND_MAX + 1.0));
}

void Enmatp::insert(const IntervalSub &sub)
{
	double width = 0;
	for (int i = 0; i < sub.size; i++){
		const IntervalCnt &cnt = sub.constraints[i];
		width += (double)(cnt.highValue-cnt.lowValue)/valDom;
	}
	width /= sub.size;
    if(!randominit){
        if (width < splitpoint){
            tama.insert(sub);
        } else {
            rein.insert(sub);
        }
    }
    else{
        if (random1() < 0.5){
            tama.insert(sub);
        } else {
            rein.insert(sub);
        }
    }
}


void Enmatp::match(const Pub &pub, const vector<IntervalSub> &subList, int &matchSubs1, int &matchSubs2, int64_t &t1, int64_t &t2)
{

	thread th1(&Enmatp::reinmatch, this, cref(pub), ref(matchSubs1), ref(t1));
	thread th2(&Enmatp::tamamatch, this, cref(pub), cref(subList), ref(matchSubs2), ref(t2));
	th1.join();
	th2.join();
    totalt1 += t1;
    totalt2 += t2;
}

void Enmatp::reinmatch(const Pub &pub, int &matchSubs, int64_t &t){
	Timer matchstart;
    rein.match(pub, matchSubs);
	t = matchstart.elapsed_nano(); 
}

void Enmatp::tamamatch(const Pub &pub, const vector<IntervalSub> &subList, int &matchSubs, int64_t &t){
	Timer matchstart;
    tama.match(pub, matchSubs, subList);
	t = matchstart.elapsed_nano(); 
}

double Enmatp::adjustsplit(){
    splitpoint = totalt1 * (maxwidth - minwidth) / (totalt1 + totalt2) + minwidth;
    totalt1 = totalt2 = 0;
    return splitpoint;
}