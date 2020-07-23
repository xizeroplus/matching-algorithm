#include <iostream>
#include <fstream>
#include "generator.h"
using namespace std;
int main(int argc, char **argv)
{
    int subs;           // Number of subscriptions.
    int pubs;           // Number of publications.
    int atts;           // Total number of attributes, i.e. dimensions.
    int cons;           // Number of constraints(predicates) in one sub.
    int m;              // Number of constraints in one pub.
    int valDom;         // Cardinality of values.
    double width;       // Width of a predicate.


	ifstream parafile;
    parafile.open(string("./paras/paras_")+string(argv[1])+string(".txt"));
    parafile >> subs >> pubs >> atts >> cons >> m >> valDom >> width;
	parafile.close();

    m = atts;

    // Initiate generator
    intervalGenerator gen(subs, pubs, atts, cons, m, valDom, width);
    gen.GenHotSubList();
    gen.GenHotPubList();
	cout<<"gen complete"<<endl;
    return 0;
}
