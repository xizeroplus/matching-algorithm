#include "rein.h"


void Rein::insert(IntervalSub sub)
{
    for (unsigned int i = 0; i < sub.size; i++)
    {
        IntervalCnt cnt = sub.constraints.at(i);
        Combo c;
        c.val = cnt.lowValue;
        c.subID = sub.id;
        data[cnt.att][0][c.val / buckStep].push_back(c);
        c.val = cnt.highValue;
        data[cnt.att][1][c.val / buckStep].push_back(c);
    }
}


void Rein::match(const Pub &pub, int &matchSubs, const vector<IntervalSub> &subList)
{
    Timer matchStart;
    vector<bool> bits (subList.size(), false);

    for (unsigned int i = 0; i < pub.size; i++)
    {
        int value = pub.pairs.at(i).value, att = pub.pairs.at(i).att, buck = value / buckStep;
        for (unsigned int k = 0; k < data[att][0][buck].size(); k++)
            if (data[att][0][buck].at(k).val > value)
                bits.at(data[att][0][buck].at(k).subID) = true;
        for (int j = buck + 1; j < bucks; j++)
            for (unsigned int k = 0; k < data[att][0][j].size(); k++)
                bits.at(data[att][0][j].at(k).subID) = true;

        for (unsigned int k = 0; k < data[att][1][buck].size(); k++)
            if (data[att][1][buck].at(k).val < value)
                bits.at(data[att][1][buck].at(k).subID) = true;
        for (int j = buck - 1; j >= 0; j--)
            for (unsigned int k = 0; k < data[att][1][j].size(); k++)
                bits.at(data[att][1][j].at(k).subID) = true;
    }

    for (unsigned int i = 0; i < subList.size(); i++)
        if (!bits.at(i))
            ++ matchSubs;
}