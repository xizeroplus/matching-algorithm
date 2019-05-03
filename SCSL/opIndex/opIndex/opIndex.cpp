#include "opIndex.h"
#include <algorithm>

void opIndex::insert(IntervalSub &x)
{
    int att = getMinFre(x);
    isPivot[att] = true;
    for (int i = 0; i < x.size; i++)
    {
        ConElement e;
        e.subID = x.id;
        e.att = x.constraints[i].att;
        e.val = x.constraints[i].lowValue;
        data[att][0][e.att % act_seg].push_back(e);
        sig[att][0][e.att % act_seg][signatureHash2(e.att)] = true;
		e.val = x.constraints[i].highValue;
        data[att][1][e.att % act_seg].push_back(e);
        sig[att][1][e.att % act_seg][signatureHash2(e.att)] = true;
    }
	++totalcount[att];
}
void opIndex::match(Pub &pub, int &matchSubs, vector<IntervalSub>& subList, vector<double> &matchDetailPub, vector<int> &matchDetailSub)
{
    Timer t;
    initCounter(subList);
    int ps = pub.size;
    int sig2[ps];
    for (int i=0; i<ps; ++i)
    {
        int att = pub.pairs[i].att;
        sig2[att] = signatureHash2(att);
    }

	for (int i = 0; i < pub.size; i++)
	{

		int piv_att = order[i];
		if (!isPivot[piv_att])
			continue;
		for (int j = 0; j < pub.size; j++)
		{
			int att = pub.pairs[j].att, value = pub.pairs[j].value;
			int hashValue = sig2[att];
			att%=act_seg;
			vector<ConElement> &data_1 = data[piv_att][0][att];
			int data_1_size = data_1.size();
			if (sig[piv_att][0][att][hashValue]) {
				for (int k=0; k<data_1_size; ++k) {
					ConElement &ce = data_1[k];
					if (ce.att == pub.pairs[j].att && ce.val <= value) {
						--counter[ce.subID];
						if (counter[ce.subID] == 0) {
							matchDetailPub.push_back(t.elapsed_nano() / 1000000.0);
							++matchSubs;
							++countlist[piv_att];
						}
					}
				}
			}

			vector<ConElement> &data_2 = data[piv_att][1][att];
			int data_2_size = data_2.size();
			if (sig[piv_att][1][att][hashValue]) {
				for (int k=0; k<data_2_size; ++k) {
					ConElement &ce = data_2[k];
					if (ce.att == pub.pairs[j].att && ce.val >= value) {
						--counter[ce.subID];
						if (counter[ce.subID] == 0) {
							matchDetailPub.push_back(t.elapsed_nano() / 1000000.0);
							++matchSubs;
							++countlist[piv_att];
						}
					}
				}
			}
		}
	}
    
}






void opIndex::initCounter(vector<IntervalSub> &subList)
{
    for (int i = 0; i < subList.size(); i++)
        counter[i] = subList[i].size << 1;
}



int opIndex::getMinFre(IntervalSub x)
{
    int tmp = x.constraints.at(0).att;
    for (int i = 1; i < x.size; i++)
        if (fre[x.constraints[i].att] < fre[tmp])
            tmp = x.constraints[i].att;
    return tmp;
}



void opIndex::calcFrequency(vector<IntervalSub> &subList)
{
    memset(fre, 0, sizeof(fre));
    for (int i = 0; i < subList.size(); i++)
        for (int j = 0; j < subList[i].size; j++)
            ++fre[subList[i].constraints[j].att];
    ofstream fileStream;
    fileStream.open("fre.txt");
    for (int i=0; i<MAX_ATTS; ++i)
        fileStream << i << '\t' << fre[i] << '\n';
    fileStream.close();
}

int opIndex::signatureHash1(int att, int val)
{
    return att * val % act_sig;
}

int opIndex::signatureHash2(int att)
{
    return att * att % act_sig;
}

void opIndex::change(){
	int maxi;
	double tmp[20],maxnum;
	for(int i=0;i<20;++i) tmp[i]=countlist[i] * 1.0 / totalcount[i];
	for(int i=0;i<20;++i){
		maxi=0;maxnum=tmp[0];
		for(int j=1;j<20;++j){
			if(tmp[j]>maxnum){
				maxnum=tmp[j];
				maxi=j;
			}
		}
		order[i]=maxi;
		tmp[maxi]=-1;
	}
	for(int i=0;i<20;++i)cout<<order[i]<<' ';
	cout<<endl;
	memset(countlist, 0, sizeof(countlist));
}

