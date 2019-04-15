#include "rein.h"

void Rein::insert(Sub sub)
{
    for (int i = 0; i < sub.size; i++)
    {
        Cnt cnt = sub.constraints[i];
        Combo c;
        c.val = cnt.value;
        c.subID = sub.id;
        if (cnt.op == 0)        // ==
        {
            data[cnt.att][0][c.val / buckStep].push_back(c);
            data[cnt.att][1][c.val / buckStep].push_back(c);
        }
        else if (cnt.op == 1)   // >=
            data[cnt.att][0][c.val / buckStep].push_back(c);
        else                    // >=
            data[cnt.att][1][c.val / buckStep].push_back(c);
    }
}

void Rein::insert(IntervalSub sub)
{
    for (int i = 0; i < sub.size; i++)
    {
        IntervalCnt cnt = sub.constraints[i];
        Combo c;
        c.val = cnt.lowValue;
        c.subID = sub.id;
        data[cnt.att][0][c.val / buckStep].push_back(c);
        c.val = cnt.highValue;
        data[cnt.att][1][c.val / buckStep].push_back(c);
    }
}

int64_t Rein::match(const Pub &pub, int &matchSubs, const vector<IntervalSub> &subList, int x)
{
    Timer subStart;
    memset(bits, 0, sizeof(bits));
    //int x = 150; // need to be calculated according to the parameters

    for (int i = 0; i < pub.size; i++)
    {
        int value = pub.pairs[i].value, att = pub.pairs[i].att, buck = value / buckStep;
        int upper = min(bucks, buck + x);
        int lower = max(0, buck - x);
        for (int k = 0; k < data[att][0][buck].size(); k++)
            if (data[att][0][buck][k].val > value)
                bits[data[att][0][buck][k].subID] = true;
        for (int j = buck + 1; j < upper; j++)
            for (int k = 0; k < data[att][0][j].size(); k++)
                bits[data[att][0][j][k].subID] = true;

        for (int k = 0; k < data[att][1][buck].size(); k++)
            if (data[att][1][buck][k].val < value)
                bits[data[att][1][buck][k].subID] = true;
        for (int j = buck - 1; j >= lower; j--)
            for (int k = 0; k < data[att][1][j].size(); k++)
                bits[data[att][1][j][k].subID] = true;
    }

    int att2value[MAX_ATTS];
    for (int i = 0; i < pub.size; i ++)
        att2value[pub.pairs[i].att] = pub.pairs[i].value;
    for (int i = 0; i < subList.size(); i++)
        if (!bits[i])
        {
            IntervalSub sub = subList[i];
            bool flag = true;
            for (int j = 0; j < sub.size; j ++)
            {
                int att = sub.constraints[j].att, pub_value = att2value[att];
                if (pub_value < sub.constraints[j].lowValue || pub_value > sub.constraints[j].highValue)
                {
                    flag = false;
                    break;
                }
            }
            if (flag)
                ++ matchSubs;
        }
    return subStart.elapsed_nano();
}

