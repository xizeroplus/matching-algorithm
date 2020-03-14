#ifndef _GENERATOR_H
#define _GENERATOR_H
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include "data_structure.h"
using namespace std;


class intervalGenerator {
	//int subs, pubs, atts, cons, m, valDom;
	int subs, pubs;
	//IntervalSub GenOneSub(int id);
	//Pub GenOnePub(int id);
	//int random(int x);
	//bool CheckExist(const vector<int> &a,int x);

public:
    vector<IntervalSub> subList;
    vector<Pub> pubList;
    

    /*intervalGenerator(int subs, int pubs, int atts, int cons, int m, int valDom) :
            subs(subs), pubs(pubs), atts(atts), cons(cons), m(m), valDom(valDom)
    {
		 srand(time(NULL));
	}*/
	intervalGenerator(int subs, int pubs):subs(subs), pubs(pubs){
	}
	//void GenSubList(); //data generation moved to create.py
	void ReadSubList();
	
	//void GenPubList();
	void ReadPubList();
};

#endif
