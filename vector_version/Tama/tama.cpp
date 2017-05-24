#include "tama.h"

void Tama::initiate(int p, int l, int r, int level)
{
    if (level == mLevel)
        return ;
    mid.at(p) = median(l, r);
    if (l != r)
    {
        lchild.at(p) = ++nodeCounter;
        initiate(nodeCounter, l, mid.at(p), level + 1);
        rchild.at(p) = ++nodeCounter;
        initiate(nodeCounter, mid.at(p) + 1, r, level + 1);
    }
}

int Tama::median(int l, int r)
{
    return (l + r) >> 1;
}

void Tama::insert(IntervalSub sub)
{
    for (int i = 0; i < sub.size; i++)
        insert(0, sub.constraints.at(i).att, sub.id, 0, valDom - 1, sub.constraints.at(i).lowValue, sub.constraints.at(i).highValue, 1);
}

void Tama::insert(int p, int att, int subID, int l, int r, int low, int high, int level)
{
    if (level == mLevel || (low <= l && high >= r) )
    {
        data[att].at(p).push_back(subID);
        return ;
    }
    if (high <= mid.at(p))
        insert(lchild.at(p), att, subID, l, mid.at(p), low, high, level + 1);
    else if (low > mid.at(p))
        insert(rchild.at(p), att, subID, mid.at(p) + 1, r, low, high, level + 1);
    else
    {
        insert(lchild.at(p), att, subID, l, mid.at(p), low, high, level + 1);
        insert(rchild.at(p), att, subID, mid.at(p) + 1, r, low, high, level + 1);
    }
}

void Tama::match(const Pub &pub, int &matchSubs, const vector<IntervalSub> &subList)
{
    counter.resize(subList.size());
    for (int i = 0; i < subList.size(); i ++)
        counter.at(i) = subList.at(i).size;
    for (int i = 0; i < pub.size; i ++)
        match(0, pub.pairs.at(i).att, 0, valDom - 1, pub.pairs.at(i).value, 1);
    for (int i = 0; i < subList.size(); i ++)
        if (counter.at(i) == 0)
            ++matchSubs;
}

void Tama::match(int p, int att, int l, int r, int value, int level)
{
    for (int i = 0; i < data[att].at(p).size(); i ++)
        -- counter.at(data[att].at(p).at(i));
    if (l == r || level == mLevel)
        return ;
    else if (value <= mid.at(p))
        match(lchild.at(p), att, l, mid.at(p), value, level + 1);
    else
        match(rchild.at(p), att, mid.at(p) + 1, r, value, level + 1);
}