#ifndef _DATA_STRUCTURE_H
#define _DATA_STRUCTURE_H
#include<string>
#include<vector>
#include<deque>
using namespace std;

struct Cnt {
    int att;
    int value;
    int op;     //  op==0 -> "="  op==1 -> ">="  op==2 -> "<="
};

struct IntervalCnt {
    int att;
    int lowValue, highValue;
};

struct Sub {
    int id;
    int size; 								//number of predicates
    vector<Cnt> constraints;				//list of constraints
};

struct IntervalSub {
    int id;//订阅id
    int size;//约束个数
    vector<IntervalCnt> constraints;//约束集
};

struct ConElement {
    int att;
    int val;
    int subID;
};

struct Combo {
    int val;
    int subID;
};

struct IntervalCombo {
    int lowValue, highValue;
    int subID;
};

struct Pair {//订阅约束
    int att;
    int value;
};

struct lowTreeValue {  
    int subId;
    int high;
};

struct Pub {
    int size;//event constraint num
    vector<Pair> pairs;// value set
};


#endif //_DATA_STRUCTURE_H
