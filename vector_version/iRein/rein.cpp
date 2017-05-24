#include "rein.h"


void Rein::insert(IntervalSub sub)
{
    for (int i = 0; i < sub.size; i++)
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


void Rein::match(const Pub &pub, int &matchSubs, const vector<IntervalSub> &subList, int x)
{
    Timer subStart;
    vector<bool> bits (subList.size(), false);

    for (int i = 0; i < pub.size; i++)
    {
        int value = pub.pairs[i].value, att = pub.pairs[i].att, buck = value / buckStep;
        int upper = min(bucks, buck + x);
        int lower = max(0, buck - x);
        for (int k = 0; k < data[att][0][buck].size(); k++)
            if (data[att][0][buck].at(k).val > value)
                bits.at(data[att][0][buck].at(k).subID) = true;
        for (int j = buck + 1; j < upper; j++)
            for (int k = 0; k < data[att][0][j].size(); k++)
                bits.at(data[att][0][j].at(k).subID) = true;

        for (int k = 0; k < data[att][1][buck].size(); k++)
            if (data[att][1][buck].at(k).val < value)
                bits.at(data[att][1][buck].at(k).subID) = true;
        for (int j = buck - 1; j >= lower; j--)
            for (int k = 0; k < data[att][1][j].size(); k++)
                bits.at(data[att][1][j].at(k).subID) = true;
    }

    int att2value[MAX_ATTS];
    for (int i = 0; i < pub.size; i ++)
        att2value[pub.pairs.at(i).att] = pub.pairs.at(i).value;
    for (int i = 0; i < subList.size(); i++)
        if (!bits.at(i))
        {
            IntervalSub sub = subList.at(i);
            bool flag = true;
            for (int j = 0; j < sub.size; j ++)
            {
                int att = sub.constraints.at(j).att, pub_value = att2value[att];
                if (pub_value < sub.constraints.at(j).lowValue || pub_value > sub.constraints.at(j).highValue)
                {
                    flag = false;
                    break;
                }
            }
            if (flag)
                ++ matchSubs;
        }
}

