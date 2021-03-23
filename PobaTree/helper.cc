#include <iostream>
#include "data_structure.h"
#include "generator.h"
#include <fstream>
using namespace std;

int main(int argc, char **argv)
{
    int subs;     // Number of subscriptions.
    int pubs;     // Number of publications.
    int atts;     // Total number of attributes, i.e. dimensions.
    int cons;     // Number of constraints(predicates) in one sub.
    int m;        // Number of constraints in one pub.
    int valDom;   // Cardinality of values.
    int attDis;   // The distribution of attributes in subs and pubs. 0:uniform distribution | 1:Zipf distribution
    int valDis;   // The distribution of values in subs and pubs. 0:uniform, 1: both zipf, -1: sub zipf but pub uniform
    double alpha; // Parameter for Zipf distribution.
    double width; // Width of a predicate.

    freopen("paras.txt", "r", stdin);
    cin >> subs >> pubs >> atts >> cons >> m >> attDis >> valDis >> valDom;
    cin >> alpha >> width;

    valDis = atoi(argv[1]);
    width = atof(argv[2]);
    alpha = atof(argv[3]);

    m = atts; // Note that Rein requires m == atts.

    char buf[128];
    snprintf(buf, 128, "subs_%d_%d_%d_%d_%d_%d_%d_%d_%f_%f",
             subs, pubs, atts, cons, m, attDis, valDis, valDom, alpha, width);

    // Initiate generator
    intervalGenerator gen(subs, pubs, atts, cons, m, attDis, valDis, valDom, alpha, width);
    cout << "generate subs begin" << buf << "\n";
    gen.GenSubList();
    cout << "generate subs end" << buf << "\n";

    cout << "generate pubs begin" << buf << "\n";
    gen.GenPubList();
    cout << "generate pubs finished" << buf << "\n";

    fstream resultFile(buf, ios::out);

    for (IntervalSub &sub : gen.subList)
    {
        resultFile << sub.id << " " << sub.size << " ";
        for (auto &constraint : sub.constraints)
        {
            resultFile << constraint.att << " " << constraint.lowValue << " " << constraint.highValue << " ";
        }
        resultFile << "\n";
    }
    resultFile.close();

     snprintf(buf, 128, "pubs_%d_%d_%d_%d_%d_%d_%d_%d_%f_%f",
              subs, pubs, atts, cons, m, attDis, valDis, valDom, alpha, width);
     fstream resultFile2(buf, ios::out);
     for (Pub &pub : gen.pubList)
     {
         resultFile2 << pub.size << " ";
         for (auto &attrVal : pub.pairs)
         {
             resultFile2 << attrVal.att << " " << attrVal.value << " ";
         }
         resultFile2 << "\n";
     }
     resultFile2.close();

    return 0;
}
