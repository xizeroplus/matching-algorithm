#include "siena.h"

void Siena::insert(IntervalSub sub)
{
    for (int i = 0; i < sub.size; i++)
    {
        IntervalCombo combo;
        combo.highValue = sub.constraints[i].highValue;
        combo.lowValue = sub.constraints[i].lowValue;
        combo.subID = sub.id;
        data[sub.constraints[i].att].push_back(combo);
    }
}

void Siena::match(const Pub &pub, int &matchSubs, const vector<IntervalSub> &subList)
{
    for (int i = 0; i < subList.size(); i++)
        counter[i] = subList[i].size;
    for (int i = 0; i < pub.size; i++)
    {
        int att = pub.pairs[i].att, value = pub.pairs[i].value;
        for (int j = 0; j < data[att].size(); j++)
            if (value >= data[att][j].lowValue && value <= data[att][j].highValue)
                --counter[data[att][j].subID];
    }
    for (int i = 0; i < subList.size(); i++)
        if (counter[i] == 0)
            ++matchSubs;
}