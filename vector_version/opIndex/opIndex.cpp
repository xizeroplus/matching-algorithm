#include "opIndex.h"


void opIndex::insert(IntervalSub x)
{
    int att = getMinFre(x);
    isPivot[att] = true;
    for (int i = 0; i < x.size; i++)
    {
        ConElement e;
        e.subID = x.id;
        e.att = x.constraints.at(i).att;
        e.val = x.constraints.at(i).lowValue;
        data[att][1][e.att % SEGMENTS][signatureHash2(e.att)].push_back(e);
        sig[att][1][e.att % SEGMENTS][signatureHash2(e.att)] = true;
        e.val = x.constraints[i].highValue;
        data[att][2][e.att % SEGMENTS][signatureHash2(e.att)].push_back(e);
        sig[att][2][e.att % SEGMENTS][signatureHash2(e.att)] = true;
    }
}


void opIndex::match(Pub pub, int &matchSubs, vector<IntervalSub>& subList)
{
    initCounter(subList);
    for (int i = 0; i < pub.size; i++)
    {
        int piv_att = pub.pairs.at(i).att;

        if (!isPivot[piv_att])
            continue;


        for (int j = 0; j < pub.size; j++)
        {
            int att = pub.pairs.at(j).att % SEGMENTS, value = pub.pairs.at(j).value;

            int hashValue = signatureHash2(pub.pairs.at(j).att);
            if (sig[piv_att][1][att][hashValue])
                for (int k = 0; k < data[piv_att][1][att][hashValue].size(); k++)
                {
                    ConElement ce = data[piv_att][1][att][hashValue].at(k);
                    if (ce.att == pub.pairs.at(j).att && ce.val <= value)
                    {
                        --counter.at(ce.subID);
                        if (counter.at(ce.subID) == 0)
                            ++matchSubs;
                    }
                }
            if (sig[piv_att][2][att][hashValue])
                for (int k = 0; k < data[piv_att][2][att][hashValue].size(); k++)
                {
                    ConElement ce = data[piv_att][2][att][hashValue].at(k);
                    if (ce.att == pub.pairs.at(j).att && ce.val >= value)
                    {
                        --counter.at(ce.subID);
                        if (counter.at(ce.subID) == 0)
                            ++matchSubs;
                    }
                }
        }


    }
}


void opIndex::initCounter(vector<IntervalSub> &subList)
{
    counter.resize(subList.size());     // maybe reserve
    for (int i = 0; i < subList.size(); i++)
        counter.at(i) = subList.at(i).size << 1;
}


int opIndex::getMinFre(IntervalSub x)
{
    int tmp = x.constraints.at(0).att;
    for (int i = 1; i < x.size; i++)
        if (fre[x.constraints.at(i).att] < fre[tmp])
            tmp = x.constraints.at(i).att;
    return tmp;
}


void opIndex::calcFrequency(vector<IntervalSub> &subList)
{
    memset(fre, 0, sizeof(fre));
    for (int i = 0; i < subList.size(); i++)
        for (int j = 0; j < subList.at(i).size; j++)
            ++fre[ subList.at(i).constraints.at(j).att];
}

int opIndex::signatureHash1(int att, int val)
{
    return att * val % MAX_SIGNATURE;
}

int opIndex::signatureHash2(int att)
{
    return att * att % MAX_SIGNATURE;
}