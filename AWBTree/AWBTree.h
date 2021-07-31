#pragma once
#ifndef _AWBTree_H
#define _AWBTree_H

#include<vector>
#include <cstring>
#include "generator.h"
#include "chrono_time.h"
#include "util.h"
#include "data_structure.h"
#include"bplustree.h"

using namespace std;

using Lowtree = BPlusTree::Tree<int, lowTreeValue>;
using Hightree = BPlusTree::Tree<int, int>;

const int MAX_SUB = 10000001;
const int MAX_WIDTH_CELL = 1001;
const int MAX_DIM = 30001;

class AWBTree {
private:
	Lowtree* lTree[MAX_DIM][MAX_WIDTH_CELL];
	Hightree* hTree[MAX_DIM][MAX_WIDTH_CELL];

	bool sigs[MAX_WIDTH_CELL];
	bool bitsets[MAX_SUB];
	unsigned short counter[MAX_SUB];
	unsigned short PDR[MAX_SUB];
	//int counter[MAX_SUB];
	//int PDR[MAX_SUB];
	//vector<int> counter;

	int check,recheck;

	int valDom;
	//total dimension
	int dim;
	//width size
	double Wsize;
	//width cell size
	unsigned short WCsize;
	// patition point
	float Ppoint;

public:
	AWBTree(const int dim, const int val, const unsigned short WCsize, float Ppoint) :
		dim(dim), valDom(val), WCsize(WCsize), Ppoint(Ppoint), Wsize((double)val/(double)WCsize)
	{
		//counter.resize(MAX_SUB);
		int branch = 2000;
		for (int i = 0; i < MAX_SUB; i++) {
			bitsets[i] = false;
			counter[i] = PDR[i] = 0;
		}
		for (int i = 0; i < dim; i++) {
			for (int j = 0; j < WCsize; j++) {
				sigs[j] = false;
				lTree[i][j] = new Lowtree(branch);
				hTree[i][j] = new Hightree(branch);
			}
		}
	}

	void insert(IntervalSub sub);
	void deleteSub(IntervalSub sub);

	void forward(const Pub& pub, int& matchSubs, const vector<IntervalSub>& subList, const int widthId);
	void forward_opt(const Pub& pub, int& matchSubs, const vector<IntervalSub>& subList, const int& widthId);
	void forward_a(const Pub& pub, int& matchSubs, const vector<IntervalSub>& subList, const int widthId);
	void backward(const Pub& pub, int& matchSubs, const vector<IntervalSub>& subList, const int widthId);
	void backward_opt(const Pub& pub, int& matchSubs, const vector<IntervalSub>& subList, const int widthId);

	//void hybrid(const Pub& pub, int& matchSubs, const vector<IntervalSub>& subList, const float Ppoint);
	void hybrid_opt(const Pub& pub, int& matchSubs, const vector<IntervalSub>& subList, const float &Ppoint);
	void hybrid_a(const Pub& pub, int& matchSubs, const vector<IntervalSub>& subList, const float &Ppoint);
	size_t mem_size();
	size_t mem_size(int i);
};

#endif // !_AWBTree
