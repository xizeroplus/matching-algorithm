#include <iostream>
#include "rein.h"
#include "oldrein.h"
using namespace std;
int main(int argc, char **argv)
{
    int subs;           // Number of subscriptions.
    int pubs;           // Number of publications.
    int attDom;           // Total number of attributes, i.e. dimensions.
    int k;           // Number of constraints(predicates) in one sub.
    int m;              // Number of constraints in one pub.
    int valDom;         // Cardinality of values.
    int subAttDis;         // The distribution of attributes in subs and pubs. 0:uniform distribution | 1:Zipf distribution
    int subValDis;         // The distribution of values in subs and pubs. 0:uniform
    int pubAttDis;         // The distribution of attributes in subs and pubs. 0:uniform distribution | 1:Zipf distribution
    int pubValDis;         // The distribution of values in subs and pubs. 0:uniform
    double subAttalpha;       // Parameter for Zipf distribution.
    double pubAttalpha;
    double subValalpha;
    double pubValalpha;
    double width;       // Width of a predicate.
    double equalRatio;
    int old_buck_num;
    int new_buck_num;

    ifstream parafile;
    parafile.open(string("./paras/paras_")+string(argv[1])+string(".txt"));
    parafile >> subs >> pubs >> k >> m >> subAttDis >> subValDis >> pubAttDis >> pubValDis >> attDom >> valDom >> subAttalpha >>subValalpha >>  pubAttalpha >> pubValalpha >> width >> equalRatio >> old_buck_num >> new_buck_num;
    parafile.close();

    //m = atts;           // Note that Rein requires m == atts.
	width=1;

	//pubValDis=1;
	//pubValalpha=1;
    // Initiate generator
    intervalGenerator gen(subs, pubs, k, m, subAttDis, subValDis, pubAttDis, pubValDis, attDom, valDom, subAttalpha ,subValalpha ,pubAttalpha, pubValalpha, width, equalRatio);
    gen.GenSubList();
    gen.GenPubList();
    cout << "gen datalist finished" << endl;
	return 0;
}
