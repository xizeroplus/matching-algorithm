#include "htree.h"


void Htree::insert(IntervalSub sub)
{
    vector<vector<int> > mark;
    mark.resize(unsigned(mLevel));
    for (int i = 0; i < sub.size; i++)
    {
        int att = sub.constraints[i].att;
        if (att < mLevel)        // Choose the first mLevel attributes as Indexed attributes
        {
            double low = sub.constraints[i].lowValue / (double) valDom, high =
                    sub.constraints[i].highValue / (double) valDom;
            int split = int(ceil((high - low) / cellStep) + 0.1);
            for (int k = 0; k < split; k++)
                mark[att].push_back(int(low / cellStep) + k);
        }

    }
    for (int i = 0; i < mLevel; i++)
        if (mark[i].size() == 0)
            mark[i].push_back(cells - 1);
    insertToBucket(mark, 0, 0, sub.id);
}


void Htree::insertToBucket(const vector<vector<int> > &mark, int bucketID, int level, int subID)
{
    if (level == mLevel - 1)
    {
        for (int i = 0; i < mark[level].size(); i++)
            buckets[bucketID + mark[level][i]].push_back(subID);
        return;
    }
    for (int i = 0; i < mark[level].size(); i++)
        insertToBucket(mark, (bucketID + mark[level][i]) * cells, level + 1, subID);
}


void Htree::match(const Pub &pub, int &matchSubs, const vector<IntervalSub> &subList)
{
    memset(matched, 0, sizeof(matched));
    vector<vector<int> > mark;
    mark.resize(unsigned(mLevel));
    for (int i = 0; i < pub.size; i++)
    {
        int att = pub.pairs[i].att;
        if (att < mLevel)
        {
            double value = pub.pairs[i].value / (double) valDom;
            if (value < cellStep)
                mark[att].push_back(0);
            else if (value > 1 - cellStep)
                mark[att].push_back(cells - 2);
            else
            {
                mark[att].push_back(int(value / cellStep));
                mark[att].push_back(int(value / cellStep) - 1);
            }
        }
    }
    for (int i = 0; i < mLevel; i++)
        mark[i].push_back(cells - 1);
    matchInBucket(mark, 0, 0, matchSubs, subList, pub);
}


void Htree::matchInBucket(const vector<vector<int> > &mark, int bucketID, int level, int &matchSubs,
                          const vector<IntervalSub> &subList, Pub pub)
{
    if (level == mLevel - 1)
    {
        int *tmp = new int[atts];
        for (int i = 0; i < atts; i++)
            tmp[i] = -1;
        for (int i = 0; i < pub.size; i++)
            tmp[pub.pairs[i].att] = i;
        for (int i = 0; i < mark[level].size(); i++)
        {
            int id = bucketID + mark[level][i];
            for (int k = 0; k < buckets[id].size(); k++)
            {
                IntervalSub sub = subList[buckets[id][k]];

                bool flag = true;
                for (int j = 0; j < sub.size; j++)
                {
                    int att = sub.constraints[j].att;
                    if (tmp[att] == -1 || pub.pairs[tmp[att]].value < sub.constraints[j].lowValue ||
                        pub.pairs[tmp[att]].value > sub.constraints[j].highValue)
                    {
                        flag = false;
                        break;
                    }
                }

                if (flag && !matched[buckets[id][k]])
                {
                    ++matchSubs;
                    matched[buckets[id][k]] = true;
                }
            }
        }
        delete[] tmp;
        return;
    }
    for (int i = 0; i < mark[level].size(); i++)
        matchInBucket(mark, (bucketID + mark[level][i]) * cells, level + 1, matchSubs, subList, pub);
}
