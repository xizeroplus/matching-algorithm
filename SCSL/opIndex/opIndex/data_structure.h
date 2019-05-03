#ifndef _DATA_STRUCTURE_H
#define _DATA_STRUCTURE_H
#include<string>
#include<vector>
#include <list>

using namespace std;
struct Cnt{
    int att;
    int value;
    int op;     //  op==0 -> "="  op==1 -> ">="  op==2 -> "<="
};

struct IntervalCnt{
    int att;
    int lowValue, highValue;
};

struct Sub {
    int id;
    int size; 								//number of predicates
    vector<Cnt> constraints;				//list of constraints
};

struct IntervalSub{
    int id;
    int size;
    vector<IntervalCnt> constraints;
};

struct ConElement {
    int att;
    int val;
    int subID;
};

struct originConElement
{
    int att;
    int val;
    int subID;
};

struct Pair{
    int att;
    int value;
};

struct Pub{
    int size;
    vector<Pair> pairs;
};

struct SubAndTime{
    int id;
    int times;
    bool operator >(const SubAndTime& rhs) const
    {
        return times > rhs.times;
    }
};

struct conIndex
{
    int att;
    int op;
    int seg;
    int sig;
    int offset;
};

struct itIndex
{
    int att;
    int op;
    int seg;
    int sig;
    list<ConElement>::iterator it;
};

struct att_times
{
    int att;
    int times;
    double ratio;
    bool operator > (const att_times &m)const {
        return times > m.times && ratio > m.ratio;
    }
    bool operator < (const att_times &m)const {
        return times < m.times && ratio < m.ratio;
    }
};

struct seg_times
{
    int att;
    int value;
    int times;
    bool operator > (const seg_times &m)const {
        return times > m.times;
    }
    bool operator < (const seg_times &m)const {
        return times < m.times;
    }
};
#endif //_DATA_STRUCTURE_H
