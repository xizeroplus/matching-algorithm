#ifndef _TAMA_H
#define _TAMA_H
#include "util.h"

using namespace std;


const int TAMA_MAX_ATTS = 21;
class Tama {
    int mLevel, atts, valDom;
    int nodeCounter;
    int *lchild, *rchild, *mid;
    vector<int> *data[TAMA_MAX_ATTS];
    vector<int> counter, defaultcounter;

    void initiate(int p, int l, int r, int level);


    void insert(int p, int att, int subID, int l, int r, int low, int high, int level);

    void match1(int p, int att, int l, int r, int value, int level, const vector<IntervalSub> &subList);

	
public:
    Tama(int atts, int valDom, int level) : atts(atts), valDom(valDom), mLevel(level), subsize(0)
    {
        for (int i = 0; i < atts; i++)
            data[i] = new vector<int>[1 << level];
        lchild = new int[1 << level];
        rchild = new int[1 << level];
        mid = new int[1 << level];
        nodeCounter = 0;
        initiate(0, 0, valDom - 1, 1);
    }
	int subsize;
    void insert(const IntervalSub &sub);

    void match(const Pub &pub, int &matchSubs, const vector<IntervalSub> &subList);
	
	
	~Tama(){
		for (int i = 0; i < atts; i++)
            delete [] data[i];
		delete [] lchild;
		delete [] rchild;
		delete [] mid;
	}

};

#endif //_TAMA_H
