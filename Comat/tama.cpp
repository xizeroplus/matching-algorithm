#include "tama.h"

inline int median(int l, int r)
{
	return (l + r) >> 1;
}

void Tama::initiate(int p, int l, int r, int level)
{
	if (level == mLevel)
		return ;
	mid[p] = median(l, r);
	if (l != r)
	{
		lchild[p] = ++nodeCounter;
		initiate(nodeCounter, l, mid[p], level + 1);
		rchild[p] = ++nodeCounter;
		initiate(nodeCounter, mid[p] + 1, r, level + 1);
	}
}


void Tama::insert(const IntervalSub &sub)
{
	for (int i = 0; i < sub.size; i++)
		insert(0, sub.constraints[i].att, sub.id, 0, valDom - 1, sub.constraints[i].lowValue, sub.constraints[i].highValue, 1);
	defaultcounter.push_back(sub.size);
	++subsize;
}

void Tama::insert(int p, int att, int subID, int l, int r, int low, int high, int level)
{
	if (level == mLevel || (low <= l && high >= r) )
	{
		data[att][p].push_back(subID);
		return ;
	}
	if (high <= mid[p])
		insert(lchild[p], att, subID, l, mid[p], low, high, level + 1);
	else if (low > mid[p])
		insert(rchild[p], att, subID, mid[p] + 1, r, low, high, level + 1);
	else
	{
		insert(lchild[p], att, subID, l, mid[p], low, high, level + 1);
		insert(rchild[p], att, subID, mid[p] + 1, r, low, high, level + 1);
	}
}

void Tama::match(const Pub &pub, int &matchSubs, const vector<IntervalSub> &subList)
{
	counter.assign(defaultcounter.begin(), defaultcounter.end());
	for (int i = 0; i < pub.size; i ++)
		match1(0, pub.pairs[i].att, 0, valDom - 1, pub.pairs[i].value, 1, subList);
	for (int i = 0; i < subsize; i ++)
		if (counter[i] == 0){
				++matchSubs;
		}
}

void Tama::match1(int p, int att, int l, int r, int value, int level, const vector<IntervalSub> &subList)
{
		if(level == mLevel){
		for (int i = 0; i < data[att][p].size(); i ++){
			const IntervalSub &sub = subList[data[att][p][i]];
			int attinsub = -1;
			for(int j=0; j<sub.size; ++j){
				if(sub.constraints[j].att==att){
					attinsub = j;
					break;
				}
			}
			if(attinsub == -1){
				-- counter[data[att][p][i]];
				continue;
			}
			if (value >= sub.constraints[attinsub].lowValue && value <= sub.constraints[attinsub].highValue){
				-- counter[data[att][p][i]];
			}
		}
		return;
	}
	for (int i = 0; i < data[att][p].size(); i ++)
		-- counter[data[att][p][i]];
	if (l == r)
		return ;
	else if (value <= mid[p])
		match1(lchild[p], att, l, mid[p], value, level + 1, subList);
	else
		match1(rchild[p], att, mid[p] + 1, r, value, level + 1, subList);
}


