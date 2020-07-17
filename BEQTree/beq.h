#ifndef _BEQ_H
#define _BEQ_H
#include "util.h"
#include<algorithm>
using namespace std;

const int MAX_PUBS = 5000001;
const int MAX_ATTS = 21;
const int EMAX = 60000;
const double lngmax=122.1757, lngmin=119.9806, latmax = 32.1031, latmin =29.9827;
class Beq {
    int atts, emax;
	int splitmap[MAX_PUBS];
    int counter[MAX_PUBS];
	beqnode root;
	void insert(const Pub &pub, beqnode &node, const vector<Pub> &pubList);
	void match(const IntervalSub &sub, beqnode &node, int &matchPubs, const vector<Pub> &pubList);
	void deletePub(const Pub &pub, beqnode &node, const vector<Pub> &pubList);
	void clearnode(beqnode &node);

public:
    Beq(int atts, int emax) : atts(atts), emax(emax)
    {
        root.cx = (lngmax+lngmin)/2;
		root.cy = (latmax+latmin)/2;
		root.rx = (lngmax-lngmin)/2;
		root.ry = (latmax-latmin)/2;
    }

    void insert(const Pub &pub, const vector<Pub> &pubList);

    void match(const IntervalSub &sub, int &matchPubs, const vector<Pub> &pubList);
	
	void deletePub(const Pub &pub, const vector<Pub> &pubList);
	
	
	~Beq(){
		clearnode(root);
	}
};

#endif
