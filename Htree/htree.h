#ifndef HTREE_H
#define HTREE_H
#include "util.h"

using namespace std;

const int MAX_SUBS = 200000;

class Htree {
    int cells, atts, mLevel, valDis, valDom;
    double cellStep;
    vector<int> *buckets;
    bool matched[MAX_SUBS];

    void insertToBucket(const vector<vector<int> > &mark, int bucketID, int level, int subID);

    void matchInBucket(const vector<vector<int> > &mark, int bucketID, int level, int &matchSubs,
                       const vector<IntervalSub> &subList, Pub pub);
    //bool match(const IntervalSub &sub, const Pub &pub);
public:
    Htree(int atts, int level, int cells, int valDis, int valDom) : atts(atts), mLevel(level), cells(cells),
                                                                    valDis(valDis), valDom(valDom)
    {
        cellStep = 1.0 / cells;
        buckets = new vector<int>[int(pow(cells, level) + 0.5)];
    }

    void insert(IntervalSub sub);

    void match(const Pub &pub, int &matchSubs, const vector<IntervalSub> &subList);

};

#endif //HTREE_H
