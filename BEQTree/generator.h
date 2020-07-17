#ifndef _GENERATOR_H
#define _GENERATOR_H
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include "data_structure.h"
using namespace std;


class intervalGenerator {
	int subs, pubs;

public:
    vector<IntervalSub> subList;
    vector<Pub> pubList;
    

	intervalGenerator(int subs, int pubs):subs(subs), pubs(pubs){
	}
	void ReadSubList();
	
	void ReadPubList();
};

#endif
