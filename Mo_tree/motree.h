#ifndef MOTREE_H
#define MOTREE_H
#include "util.h"

using namespace std;

const int MAX_SUBS = 1000000;
const int MAX_LEVELS = 10;
const double LNGMIN=121.2, LNGMAX=121.7, LATMIN=31.0, LATMAX=31.4, LNGCELL=0.1, LATCELL=0.08; //Shanghai center area (total data about 120-122 30-32)

class Motree {
    int mLevel, valDom, level,subcells, pubcells;
	double levelTarget;
    //bool matched[MAX_SUBS];
    //string contentstr;
    vector<vector<int> > subdata, pubdata;
	double overlap[MAX_LEVELS], cellWidth[MAX_LEVELS], startDis[MAX_LEVELS];
    void insertToBucket(const vector<int> &mark, int bucketID, int layer, int subID);
	void insertToPubdata(const vector<int> &mark, int bucketID, int layer, int pubID);

    void matchInBucket(const vector<vector<int> > &mark, int bucketID, int layer, int &matchSubs, const vector<IntervalSub> &subList, const Pub &pub);
	void matchInPubdata(const vector<vector<int> > &mark, int bucketID, int layer, int &matchPubs, const vector<Pub> &pubList, const IntervalSub &sub);
	
	void deleteFromBucket(const vector<int> &mark, int bucketID, int layer, int subID);
	void deleteFromPubdata(const vector<int> &mark, int bucketID, int layer, int pubID);

public:
    Motree(int layer = 6, int level = 4, int valDom = 1000000) : mLevel(layer), level(level), valDom(valDom), pubcells(21), subcells(level*(level+1)/2), subdata((int)(pow(subcells, layer-2)*10*10 + 0.5)), pubdata((int)(pow(pubcells, layer) + 0.5))
    {

		levelTarget = 1.0 / level;
		cellWidth[0] = 1.0;
		startDis[0] = 1.0;
		for(int i = 1; i < level; ++i){
			overlap[i] = levelTarget * (level-i);
			cellWidth[i] = (overlap[i]*i+1)/(i+1);
			startDis[i] = (1-cellWidth[i])/i;
		}
    }

    void insertSub(const IntervalSub &sub, int &matchPubs, int64_t &insertTime, int64_t &matchTime, const vector<Pub> &pubList);

    void insertPub(const Pub &pub, int &matchSubs, int64_t &insertTime, int64_t &matchTime, const vector<IntervalSub> &subList);
	
	void deleteSub(const IntervalSub &sub, int64_t &deleteTime);
	
	void deletePub(const Pub &pub, int64_t &deleteTime);
	
	/*~Motree(){
		delete [] subdata;
		delete [] pubdata;
	}*/
};

#endif