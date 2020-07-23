#include "rein.h"


void Rein::insert(const IntervalSub &sub)
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
	++subsize;
}



void Rein::match(const Pub &pub, int &matchSubs)
{
    vector<bool> bits (subsize, false);

    for (int i = 0; i < pub.size; i++)
    {
        int value = pub.pairs[i].value, att = pub.pairs[i].att, buck = value / buckStep;
        for (int k = 0; k < data[att][0][buck].size(); k++)
            if (data[att][0][buck][k].val > value)
                bits[data[att][0][buck][k].subID] = true;
        for (int j = buck + 1; j < bucks; j++)
            for (int k = 0; k < data[att][0][j].size(); k++)
                bits[data[att][0][j][k].subID] = true;

        for (int k = 0; k < data[att][1][buck].size(); k++)
            if (data[att][1][buck][k].val < value)
                bits[data[att][1][buck][k].subID] = true;
        for (int j = buck - 1; j >= 0; j--)
            for (int k = 0; k < data[att][1][j].size(); k++)
                bits[data[att][1][j][k].subID] = true;
    }

    for (int i = 0; i < subsize; i++)
        if (!bits[i]){
            ++matchSubs;
		}
}
