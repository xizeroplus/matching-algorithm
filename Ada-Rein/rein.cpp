#include "rein.h"


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


void Rein::select_skipped_atts(const vector<IntervalSub> &subList, int atts, double falsePositive)
{
    for (int i = 0; i < atts; i++)
    {
        attsCounts[i].att = i;
        attsCounts[i].count = 0;
    }
    int countSum = 0, currentSum = 0;
    for (int i = 0; i < subList.size(); i++)
        for (int j = 0; j < subList[i].size; j++)
        {
            ++attsCounts[subList[i].constraints[j].att].count;
            ++countSum;
        }
    sort(attsCounts, attsCounts + atts);

    memset(skipped, 0, sizeof(skipped));
    for (int i = 0; i < atts; i++)
    {
        currentSum += attsCounts[i].count;
        if ((double)(countSum - currentSum) / (double)subList.size() > subList[0].constraints.size() + log(falsePositive + 1) / log((subList[0].constraints[0].highValue - subList[0].constraints[0].lowValue) / (double)1000000))
            skipped[attsCounts[i].att] = true;
        else
        {
            cout << i << endl;
            break;
        }

    }
}


int64_t Rein::approx_match(const Pub &pub, int &matchSubs, const vector<IntervalSub> &subList)
{

    memset(bits, 0, sizeof(bits));

    Timer start;
    for (int i = 0; i < pub.size; i++)
    {
        int att = pub.pairs[i].att;
        if (skipped[att])
            continue;
        int value = pub.pairs[i].value, buck = value / buckStep;
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
    int64_t markingTime = start.elapsed_nano();

    for (int i = 0; i < subList.size(); i++)
        if (!bits[i])
            ++ matchSubs;

    return markingTime;
}

int64_t Rein::match(const Pub &pub, int &matchSubs, const vector<IntervalSub> &subList)
{
    memset(bits, 0, sizeof(bits));

    Timer matchStart;

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
    int64_t markingTime = matchStart.elapsed_nano();

    for (int i = 0; i < subList.size(); i++)
        if (!bits[i])
            ++ matchSubs;

    return  markingTime;
}
