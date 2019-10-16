#include <iostream>
#include "generator.h"

using namespace std;
int main(int argc, char **argv)
{
    int subs;           // Number of subscriptions.
    int pubs;           // Number of publications.
    int attDom;           // Total number of attributes, i.e. dimensions.
    int k;           // Number of constraints(predicates) in one sub.
    int m;              // Number of constraints in one pub.
    int valDom;         // Cardinality of values.
    int old_buck_num;
    int new_buck_num;
	double limitscale; //adjust limit
	int newlevel; //number of levels
	int cstep; //adjustment time window
    ifstream parafile;
    parafile.open(string("./paras/paras_")+string(argv[1])+string(".txt"));
    parafile >> subs >> pubs >> k >> m >> attDom >> valDom >> old_buck_num >> new_buck_num >> limitscale >> newlevel >> cstep;
    parafile.close();




    // Initiate generator
    intervalGenerator gen(subs, pubs, k, m, 0, 0, 0, 1, attDom, valDom, 1, 1, 1, 1, 0.5, 0.3);
    gen.GenSubList();
    gen.GenPubList();
    cout << "gen datalist finished" << endl;
	return 0;
}
