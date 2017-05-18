#include "opIndex.h"

void opIndex::insert(Sub x)
{
    int att = getMinFre(x);
    isPivot[att] = true;
    for (int i = 0; i < x.size; i++)
    {
        ConElement e;
        e.subID = x.id;
        e.att = x.constraints[i].att;
        e.val = x.constraints[i].value;

        if (x.constraints[i].op == 0)
        {
            data[att][x.constraints[i].op][e.att % SEGMENTS][signatureHash1(e.att, e.val)].push_back(e);
            sig[att][x.constraints[i].op][e.att % SEGMENTS][signatureHash1(e.att, e.val)] = true;
        } else
        {
            data[att][x.constraints[i].op][e.att % SEGMENTS][signatureHash2(e.att)].push_back(e);
            sig[att][x.constraints[i].op][e.att % SEGMENTS][signatureHash2(e.att)] = true;
        }
    }
}

void opIndex::insert(IntervalSub x)
{
    int att = getMinFre(x);
    isPivot[att] = true;
    for (int i = 0; i < x.size; i++)
    {
        ConElement e;
        e.subID = x.id;
        e.att = x.constraints[i].att;
        e.val = x.constraints[i].lowValue;
        data[att][1][e.att % SEGMENTS][signatureHash2(e.att)].push_back(e);
        sig[att][1][e.att % SEGMENTS][signatureHash2(e.att)] = true;
        e.val = x.constraints[i].highValue;
        data[att][2][e.att % SEGMENTS][signatureHash2(e.att)].push_back(e);
        sig[att][2][e.att % SEGMENTS][signatureHash2(e.att)] = true;
    }
}

void opIndex::match(Pub pub, int &matchSubs, vector<Sub>& subList)
{
    initCounter(subList);
    for (int i = 0; i < pub.size; i++)
    {
        int piv_att = pub.pairs[i].att;

        if (!isPivot[piv_att])
            continue;


        for (int j = 0; j < pub.size; j++)
        {
            int att = pub.pairs[j].att % SEGMENTS, value = pub.pairs[j].value;
            int hashValue = signatureHash1(pub.pairs[j].att, value);
            if (sig[piv_att][0][att][hashValue])
                for (int k = 0; k < data[piv_att][0][att][hashValue].size(); k++)
                {
                    ConElement ce = data[piv_att][0][att][hashValue][k];
                    if (ce.val == value && ce.att == pub.pairs[j].att)
                    {
                        --counter[ce.subID];
                        if (counter[ce.subID] == 0)
                            ++matchSubs;
                    }
                }
            hashValue = signatureHash2(pub.pairs[j].att);
            if (sig[piv_att][1][att][hashValue])
                for (int k = 0; k < data[piv_att][1][att][hashValue].size(); k++)
                {
                    ConElement ce = data[piv_att][1][att][hashValue][k];
                    if (ce.att == pub.pairs[j].att && ce.val <= value)
                    {
                        --counter[ce.subID];
                        if (counter[ce.subID] == 0)
                            ++matchSubs;
                    }
                }
            if (sig[piv_att][2][att][hashValue])
                for (int k = 0; k < data[piv_att][2][att][hashValue].size(); k++)
                {
                    ConElement ce = data[piv_att][2][att][hashValue][k];
                    if (ce.att == pub.pairs[j].att && ce.val >= value)
                    {
                        --counter[ce.subID];
                        if (counter[ce.subID] == 0)
                            ++matchSubs;
                    }
                }
        }
    }
}

void opIndex::match(Pub pub, int &matchSubs, vector<IntervalSub>& subList)
{
    initCounter(subList);
    for (int i = 0; i < pub.size; i++)
    {
        int piv_att = pub.pairs[i].att;
        if (!isPivot[piv_att])
            continue;

        for (int j = 0; j < pub.size; j++)
        {
            int att = pub.pairs[j].att % SEGMENTS, value = pub.pairs[j].value;

            int hashValue = signatureHash2(pub.pairs[j].att);
            if (sig[piv_att][1][att][hashValue])
                for (int k = 0; k < data[piv_att][1][att][hashValue].size(); k++)
                {
                    ConElement ce = data[piv_att][1][att][hashValue][k];
                    if (ce.att == pub.pairs[j].att && ce.val <= value)
                    {
                        --counter[ce.subID];
                        if (counter[ce.subID] == 0)
                            ++matchSubs;
                    }
                }
            if (sig[piv_att][2][att][hashValue])
                for (int k = 0; k < data[piv_att][2][att][hashValue].size(); k++)
                {
                    ConElement ce = data[piv_att][2][att][hashValue][k];
                    if (ce.att == pub.pairs[j].att && ce.val >= value)
                    {
                        --counter[ce.subID];
                        if (counter[ce.subID] == 0)
                            ++matchSubs;
                    }
                }
        }
    }
}

void opIndex::initCounter(vector<Sub> &subList)
{
    for (int i = 0; i < subList.size(); i++)
        counter[i] = subList[i].size;
}

void opIndex::initCounter(vector<IntervalSub> &subList)
{
    for (int i = 0; i < subList.size(); i++)
        counter[i] = subList[i].size << 1;
}

int opIndex::getMinFre(Sub x)
{
    int tmp = x.constraints.at(0).att;
    for (int i = 1; i < x.size; i++)
        if (fre[x.constraints[i].att] < fre[tmp])
            tmp = x.constraints[i].att;
    return tmp;
}

int opIndex::getMinFre(IntervalSub x)
{
    int tmp = x.constraints.at(0).att;
    for (int i = 1; i < x.size; i++)
        if (fre[x.constraints[i].att] < fre[tmp])
            tmp = x.constraints[i].att;
    return tmp;
}

void opIndex::calcFrequency(vector<Sub> &subList)
{
    memset(fre, 0, sizeof(fre));
    for (int i = 0; i < subList.size(); i++)
        for (int j = 0; j < subList[i].size; j++)
            ++fre[subList[i].constraints[j].att];
}

void opIndex::calcFrequency(vector<IntervalSub> &subList)
{
    memset(fre, 0, sizeof(fre));
    for (int i = 0; i < subList.size(); i++)
        for (int j = 0; j < subList[i].size; j++)
            ++fre[subList[i].constraints[j].att];
}

int opIndex::signatureHash1(int att, int val)
{
    return att * val % MAX_SIGNATURE;
}

int opIndex::signatureHash2(int att)
{
    return att * att % MAX_SIGNATURE;
}