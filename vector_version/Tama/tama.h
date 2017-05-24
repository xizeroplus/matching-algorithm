#ifndef TAMA_H
#define TAMA_H
#include "util.h"

using namespace std;

//const int tMAX_SUBS = 2000000;
const int tMAX_ATTS = 101;

class Tama{
    int mLevel, atts, valDom;
    int nodeCounter;
    vector<int> lchild, rchild, mid;
    vector< vector<int> > data[tMAX_ATTS];
    vector<int> counter;
    void initiate(int p, int l, int r, int level);
    int median(int l, int r);
    void insert(int p, int att, int subID, int l, int r, int low, int high, int level);
    void match(int p, int att, int l, int r, int value, int level);
public:
    Tama(int atts, int valDom, int level):atts(atts), valDom(valDom), mLevel(level)
    {
        for (int i = 0; i < atts; i++)
            data[i].resize(1 << level);
        lchild.resize(1 << level);
        rchild.resize(1 << level);
        mid.resize(1 << level);
        nodeCounter = 0;
        initiate(0, 0, valDom - 1, 1);
    }
    void insert(IntervalSub sub);
    void match(const Pub &pub, int &matchSubs, const vector<IntervalSub> &subList);
};

#endif //_TAMA_H
