#include "AWBTree.h"
#include <xmmintrin.h>
#include <emmintrin.h>

void AWBTree::insert(IntervalSub sub)
{
	float PWid = WCsize * Ppoint;
	for (auto pred : sub.constraints) {
		lowTreeValue v = { sub.id, pred.highValue };
		unsigned short Wid = (pred.highValue - pred.lowValue) / Wsize;
		if (Wid < PWid)PDR[sub.id]++;
		sigs[Wid] = true;

		lTree[pred.att][Wid]->insert(pred.lowValue, v);
		hTree[pred.att][Wid]->insert(pred.highValue, sub.id);
	}
}

void AWBTree::deleteSub(IntervalSub sub)
{
	for (auto pred : sub.constraints) {
		lowTreeValue v = { sub.id, pred.highValue };
		unsigned short Wid = (pred.highValue - pred.lowValue) / Wsize;
		lTree[pred.att][Wid]->erase(pred.lowValue, v);
		hTree[pred.att][Wid]->erase(pred.highValue, sub.id);
	}
}

void AWBTree::forward(const Pub& pub, int& matchSubs, const vector<IntervalSub>& subList, const int widthId)
{
	
	for(int i =0; i<subList.size();i++)counter[i]=0;
	for (auto pred : pub.pairs) {
		int i;
		for (i = 0; i < widthId; i++) {
			if (sigs[i]) {
				double Wmin = i * Wsize;
				double Wmax = (i + 1) * Wsize;
				Lowtree::iterator start = lTree[pred.att][i]->upper_bound(pred.value - Wmax);
				Lowtree::iterator mid = lTree[pred.att][i]->lower_bound(pred.value - Wmin);
				Lowtree::iterator end = lTree[pred.att][i]->upper_bound(pred.value);
				Lowtree::iterator cur;
				for (cur = start; cur != mid; cur++)
					if (pred.value <= cur.get_val().high)++counter[cur.get_val().subId];
				for (cur; cur != end; cur++)
					++counter[cur.get_val().subId];
			}
		}
	}

	//after process
	for (int i = 0; i < subList.size(); i++) {
		if (counter[i] == subList[i].size)++matchSubs;
	}
}

void AWBTree::forward_opt(const Pub& pub, int& matchSubs, const vector<IntervalSub>& subList, const int& widthId)
{
	for (auto pred : pub.pairs) {
		int i = 0;
		
		for (i; i < WCsize / 2; i++) {
			if (sigs[i]) {
				double Wmin = i * Wsize;
				double Wmax = (i + 1) * Wsize;
				double Vmax = valDom - Wmin;
				Lowtree::iterator start = lTree[pred.att][i]->upper_bound(pred.value - Wmax);
				Lowtree::iterator mid = lTree[pred.att][i]->lower_bound(pred.value - Wmin);
				Lowtree::iterator end;
				if(pred.value < Vmax)end = lTree[pred.att][i]->upper_bound(pred.value);
				else end = lTree[pred.att][i]->end();
				Lowtree::iterator cur;
				for (cur = start; cur != mid; cur++)
					if (pred.value <= cur.get_val().high){
						++counter[cur.get_val().subId];
					}
				for (cur; cur != end; cur++){
					++counter[cur.get_val().subId];
				}
			}
		}
		
		for (i; i < widthId; i++) {
			if (sigs[i]) {
				
				double Wmin = i * Wsize;
				double Wmax = (i + 1) * Wsize;
				double Vmax = valDom - Wmin;


				Lowtree::iterator start;
				Lowtree::iterator mid;
				Lowtree::iterator end;
				Lowtree::iterator cur;

				if (pred.value > Wmin) {
					start = lTree[pred.att][i]->upper_bound(pred.value - Wmax);
					mid = lTree[pred.att][i]->lower_bound(pred.value - Wmin);
					end = lTree[pred.att][i]->end();
					for (cur = start; cur != mid; cur++)
						if (pred.value <= cur.get_val().high){
							++counter[cur.get_val().subId];
						}
					for (cur; cur != end; cur++){
						++counter[cur.get_val().subId];
					}
				}
				else if (pred.value > Vmax) {
					start = lTree[pred.att][i]->begin();
					end = lTree[pred.att][i]->end();
					for (cur = start; cur != end; cur++){
						++counter[cur.get_val().subId];
					}
				}
				else {
					start = lTree[pred.att][i]->begin();
					end = lTree[pred.att][i]->upper_bound(pred.value);
					for (cur = start; cur != end; cur++){
						++counter[cur.get_val().subId];
					}
				}
			}
		}
	}

	//after process
	for (int i = 0; i < subList.size(); i++) {
		if (counter[i] == subList[i].size)++matchSubs;
		counter[i] = 0;
	}
}

void AWBTree::forward_a(const Pub& pub, int& matchSubs, const vector<IntervalSub>& subList, const int widthId)
{
	for (auto pred : pub.pairs) {
		int i;
		for (i = 0; i <  WCsize / 2; i++) {
			if (sigs[i]) {
				double Wmin = i * Wsize;
				double Wmax = (i + 1) * Wsize;
				Lowtree::iterator start = lTree[pred.att][i]->lower_bound(pred.value - Wmin);
				Lowtree::iterator end = lTree[pred.att][i]->upper_bound(pred.value);
				Lowtree::iterator cur;

				for (cur = start; cur != end; cur++)
					++counter[cur.get_val().subId];
			}
		}
		for (i; i < 18*widthId/25; i++) {
			if (sigs[i]) {

				double Wmin = i * Wsize;
				double Wmax = (i + 1) * Wsize;
				double Vmax = valDom - Wmin;

				Lowtree::iterator start;
				Lowtree::iterator mid;
				Lowtree::iterator end;
				Lowtree::iterator cur;

				if (pred.value > Wmin) {
					start = lTree[pred.att][i]->upper_bound(pred.value - Wmax);
					mid = lTree[pred.att][i]->lower_bound(pred.value - Wmin);
					end = lTree[pred.att][i]->end();
					for (cur = start; cur != mid; cur++)
						if (pred.value <= cur.get_val().high){
							++counter[cur.get_val().subId];
						}
					for (cur; cur != end; cur++)
						++counter[cur.get_val().subId];
				}
				else if (pred.value > Vmax) {
					start = lTree[pred.att][i]->begin();
					end = lTree[pred.att][i]->end();
					for (cur = start; cur != end; cur++)
						++counter[cur.get_val().subId];
				}
				else {
					start = lTree[pred.att][i]->begin();
					end = lTree[pred.att][i]->upper_bound(pred.value);
					for (cur = start; cur != end; cur++)
						++counter[cur.get_val().subId];
				}
			}	
		}
		for (i; i < widthId; i++) {
			if (sigs[i]) {

				double Wmin = i * Wsize;
				double Wmax = (i + 1) * Wsize;
				double Vmax = valDom - Wmin;

				Lowtree::iterator start;
				Lowtree::iterator end;
				Lowtree::iterator cur;

				if (pred.value > Wmin) {
					start = lTree[pred.att][i]->lower_bound(pred.value - Wmin);
					end = lTree[pred.att][i]->end();
					for (cur = start; cur != end; cur++)
						++counter[cur.get_val().subId];
				}
				else if (pred.value > Vmax) {
					start = lTree[pred.att][i]->begin();
					end = lTree[pred.att][i]->end();
					for (cur = start; cur != end; cur++)
						++counter[cur.get_val().subId];
				}
				else {
					start = lTree[pred.att][i]->begin();
					end = lTree[pred.att][i]->upper_bound(pred.value);
					for (cur = start; cur != end; cur++)
						++counter[cur.get_val().subId];
				}
			}
		}
	}

	//after process
	for (int i = 0; i < subList.size(); i++) {
		if (counter[i] == subList[i].size)++matchSubs;
		counter[i] = 0;
	}
}



void AWBTree::backward(const Pub& pub, int& matchSubs, const vector<IntervalSub>& subList, const int widthId)
{
	vector<bool> bits(subList.size() + 1, false);
	for (auto pred : pub.pairs) {
		for (int i = widthId; i < WCsize; i++) {
			if (sigs[i]) {
				Lowtree::iterator itlow = lTree[pred.att][i]->upper_bound(pred.value);
				Hightree::iterator ithigh = hTree[pred.att][i]->lower_bound(pred.value);
				for (itlow; itlow != lTree[pred.att][i]->end(); itlow++)
					bits[itlow.get_val().subId] = true;
				for (Hightree::iterator cur = hTree[pred.att][i]->begin(); cur != ithigh; cur++)
					bits[cur.get_val()] = true;
			}
		}
	}

	//after process
	for (int i = 0; i < subList.size(); i++) {
		if (!bits[i])++matchSubs;
	}
}

void AWBTree::backward_opt(const Pub& pub, int& matchSubs, const vector<IntervalSub>& subList, const int widthId)
{
	
	//Timer t;
	//int64_t backTime = 0;
	//int64_t travelTime = 0;
	vector<bool> bits(subList.size(), false);
	//memset(bitsets, 0, sizeof(bitsets));
	for (auto pred : pub.pairs) {
		int i;
		Lowtree::iterator itlow;
		Hightree::iterator ithigh;
		for (i = widthId; i < WCsize / 2; i++) {
			if (sigs[i]) {
				itlow = lTree[pred.att][i]->upper_bound(pred.value);
				ithigh = hTree[pred.att][i]->lower_bound(pred.value);
				for (itlow; itlow != lTree[pred.att][i]->end(); itlow++){
					//bitsets[itlow.get_val().subId] = true;
					bits[itlow.get_val().subId] = true;
				}
				for (Hightree::iterator cur = hTree[pred.att][i]->begin(); cur != ithigh; cur++){
					//bitsets[cur.get_val()] = true;
					bits[cur.get_val()] = true;
				}
			}
		}
		for (i; i < WCsize; i++) {
			if (sigs[i]) {
				double Vmin = i * Wsize;
				double Vmax = valDom - Vmin;
				if(pred.value<=Vmax){
					itlow = lTree[pred.att][i]->upper_bound(pred.value);
					for (itlow; itlow != lTree[pred.att][i]->end(); itlow++)
					{	//bitsets[itlow.get_val().subId] = true;
						bits[itlow.get_val().subId] = true;
					}
				}
				else if (pred.value >= Vmin) {
					ithigh = hTree[pred.att][i]->lower_bound(pred.value);
					for (Hightree::iterator cur = hTree[pred.att][i]->begin(); cur != ithigh; cur++)
					{	//bitsets[cur.get_val()] = true;
						bits[cur.get_val()] = true;
					}
				}
			}
		}
	}
	//backTime += t.elapsed_nano();
	//t.reset();
	//after process
	for (int i = 0; i < subList.size(); i++) {
		//if (!bitsets[i])++matchSubs;
		//else bitsets[i] = false;
		if (!bits[i])++matchSubs;
	}
	//travelTime = t.elapsed_nano();
	//cout << "back time: " << (double)backTime / 1000000 << "  travel time: " << (double)travelTime / 1000000 << endl;
}

void AWBTree::hybrid_opt(const Pub& pub, int& matchSubs, const vector<IntervalSub>& subList, const float &Ppoint)
{
	//Timer t;
	//int64_t backTime = 0;
	//int64_t forwardTime = 0;
	//int64_t travelTime = 0;
	vector<bool> bits(subList.size(), false);
	for (auto pred : pub.pairs) {
		int i;
		//forward
		//t.reset();
		for (i = 0; i < WCsize * Ppoint; i++) {
			if (sigs[i]) {
				double Wmin = i * Wsize;
				double Wmax = (i + 1) * Wsize;
				double Vmax = valDom - Wmin;
				Lowtree::iterator start = lTree[pred.att][i]->upper_bound(pred.value - Wmax);
				Lowtree::iterator mid = lTree[pred.att][i]->lower_bound(pred.value - Wmin);
				Lowtree::iterator end;
				if(pred.value < Vmax) end = lTree[pred.att][i]->upper_bound(pred.value);
				else end = lTree[pred.att][i]->end(); 
				Lowtree::iterator cur;
				for (cur = start; cur != mid; cur++)
					//if (pred.value <= cur->second.high)++counter[cur->second.subId];
					if (pred.value <= cur.get_val().high)++counter[cur.get_val().subId];
				for (cur; cur != end; cur++)
					//++counter[cur->second.subId];
					++counter[cur.get_val().subId];
			}
		}
		//forwardTime += t.elapsed_nano();
		//t.reset();
		//backward
		Lowtree::iterator itlow;
		Hightree::iterator ithigh;
		for (i; i < WCsize / 2; i++) {
			if (sigs[i]) {
				itlow = lTree[pred.att][i]->upper_bound(pred.value);
				ithigh = hTree[pred.att][i]->lower_bound(pred.value);
				for (itlow; itlow != lTree[pred.att][i]->end(); itlow++)
					bits[itlow.get_val().subId] = true;
				for (Hightree::iterator cur = hTree[pred.att][i]->begin(); cur != ithigh; cur++)
					bits[cur.get_val()] = true;
			}
		}
		for (i; i < WCsize; i++) {
			if (sigs[i]) {
				double Vmin = i * Wsize;
				double Vmax = valDom - Vmin;
				if (pred.value <= Vmax) {
					itlow = lTree[pred.att][i]->upper_bound(pred.value);
					for (itlow; itlow != lTree[pred.att][i]->end(); itlow++)
						bits[itlow.get_val().subId] = true;
				}
				else if (pred.value >= Vmin) {
					ithigh = hTree[pred.att][i]->lower_bound(pred.value);
					for (Hightree::iterator cur = hTree[pred.att][i]->begin(); cur != ithigh; cur++)
						bits[cur.get_val()] = true;
				}
			}
		}
		//backTime += t.elapsed_nano();
	}
	//t.reset();
	//after process
	for (int i = 0; i < subList.size(); i++) {
		if (!((PDR[i] - counter[i]) | bits[i])) {
			//if(counter[i] == PDR[i])++matchSubs;
			++matchSubs;
		}
		counter[i] = 0;
	}
	//travelTime += t.elapsed_nano();
	//cout << "forward time: " << (double)forwardTime / 1000000 << "  back time: " << (double)backTime / 1000000 << "  travel time: " << (double)travelTime / 1000000 << endl;
}

void AWBTree::hybrid_a(const Pub& pub, int& matchSubs, const vector<IntervalSub>& subList, const float &Ppoint)
{
	//Timer t;
	//int64_t backTime = 0;
	//int64_t forwardTime = 0;
	//int64_t travelTime = 0;
	vector<bool> bits(subList.size(), false);
	for (auto pred : pub.pairs) {
		int i;
		//forward
		//t.reset();
		for (i = 0; i < WCsize * Ppoint; i++) {
			if (sigs[i]) {
				double Wmin = i * Wsize;
				double Wmax = (i + 1) * Wsize;
				double Vmax = valDom - Wmin;
				//Lowtree::iterator start = lTree[pred.att][i]->lower_bound(pred.value - Wmin);
				Lowtree::iterator start = lTree[pred.att][i]->upper_bound(pred.value - Wmax);
				Lowtree::iterator end;
				if(pred.value < Vmax) end = lTree[pred.att][i]->upper_bound(pred.value);
				else end = lTree[pred.att][i]->end(); 
				Lowtree::iterator cur;
				for (cur = start; cur != end; cur++)
					++counter[cur.get_val().subId];
			}
		}
		//backward
		Lowtree::iterator itlow;
		Hightree::iterator ithigh;
		for (i; i < WCsize / 2; i++) {
			if (sigs[i]) {
				itlow = lTree[pred.att][i]->upper_bound(pred.value);
				ithigh = hTree[pred.att][i]->lower_bound(pred.value);
				for (itlow; itlow != lTree[pred.att][i]->end(); itlow++)
					bits[itlow.get_val().subId] = true;
				for (Hightree::iterator cur = hTree[pred.att][i]->begin(); cur != ithigh; cur++)
					bits[cur.get_val()] = true;
			}
		}
		for (i; i < WCsize; i++) {
			if (sigs[i]) {
				double Vmin = i * Wsize;
				double Vmax = valDom - Vmin;
				if (pred.value <= Vmax) {
					itlow = lTree[pred.att][i]->upper_bound(pred.value);
					for (itlow; itlow != lTree[pred.att][i]->end(); itlow++)
						bits[itlow.get_val().subId] = true;
				}
				else if (pred.value >= Vmin) {
					ithigh = hTree[pred.att][i]->lower_bound(pred.value);
					for (Hightree::iterator cur = hTree[pred.att][i]->begin(); cur != ithigh; cur++)
						bits[cur.get_val()] = true;
				}
			}
		}
		//backTime += t.elapsed_nano();
	}
	//t.reset();
	//after process
	for (int i = 0; i < subList.size(); i++) {
		if (!(bits[i] | (PDR[i] - counter[i]))) {
			++matchSubs;
		}
		counter[i] = 0;
	}
	//travelTime += t.elapsed_nano();
	//cout << "forward time: " << (double)forwardTime / 1000000 << "  back time: " << (double)backTime / 1000000 << "  travel time: " << (double)travelTime / 1000000 << endl;
}

size_t AWBTree::mem_size()
{
	size_t sum = 0;
	for (int i = 0; i < dim; i++) {
		sum += mem_size(i);
	}
	return sum;
}

size_t AWBTree::mem_size(int i)
{
	size_t sum = (sizeof(Lowtree*) + sizeof(Hightree*)) * WCsize;
	for (int j = 0; j < WCsize; j++) {
		sum += lTree[i][j]->mem_size();
		sum += hTree[i][j]->mem_size();
	}
	return sum;
}


