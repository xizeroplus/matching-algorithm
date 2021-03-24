#include "enmat.h"

static inline double random1()
{
    return (rand() / (RAND_MAX + 1.0));
}


void Enmat::insert(const IntervalSub &sub)
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


void Enmat::match(const Pub &pub, const vector<IntervalSub> &subList, int &matchSubs1, int &matchSubs2, int64_t &t1, int64_t &t2)
{
    Timer matchstart;
    rein.match(pub, matchSubs1);
	t1 = matchstart.elapsed_nano(); 
    matchstart.reset();
    tama.match(pub, matchSubs2, subList);
	t2 = matchstart.elapsed_nano(); 
}
