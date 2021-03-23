#ifndef _REIN_H
#define _REIN_H
#include <vector>
#include <cstring>
#include "generator.h"
#include "chrono_time.h"
#include "util.h"
#include "data_structure.h"
#include "bplustree.h"

const int MAX_SUBS = 2000001;
const int MAX_ATTS = 101;
const int MAX_BUCKS = 500;

using IntBplusTree = BPlusTree::Tree<int, int>;

struct median_search_struct
{
    size_t offset;
    vector<BPlusTree::Node<int, int> *> relative_nodes;
    bool working_model; // true: from offset to end; false: from 0 to offset

    bool operator<(const median_search_struct &b)
    {
        return relative_nodes.size() > b.relative_nodes.size();
    }

    median_search_struct()
    {
        relative_nodes.reserve(2000);
    }
};

class PobaTree
{
    vector<pair<IntBplusTree *, IntBplusTree *> > data;

public:
    PobaTree(int attr_num, size_t degree)
    {
        data.resize(attr_num);
        for (int i = 0; i < attr_num; i++)
        {
            data[i].first = new IntBplusTree(degree);
            data[i].second = new IntBplusTree(degree);
        }
    }

    void insert(IntervalSub sub);
    void deleteSub(IntervalSub sub);
    void match(const Pub &pub, int &matchSubs, const vector<IntervalSub> &subList);
    void match_parallel(const Pub &pub, int &matchSubs, const vector<IntervalSub> &subList);
    size_t mem_size();
};

#endif
