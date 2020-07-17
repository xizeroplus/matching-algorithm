#include <iostream>
#include "beq.h"
using namespace std;
int main(int argc, char **argv)
{
    int subs;           // Number of subscriptions.
    int pubs;           // Number of publications.
    int atts;           // Total number of attributes, i.e. dimensions.
    int cons;           // Number of constraints(predicates) in one sub.
    int m;              // Number of constraints in one pub.
    int valDom;         // Cardinality of values.
    int attDis;         // The distribution of attributes in subs and pubs. 0:uniform distribution | 1:Zipf distribution
    int valDis;         // The distribution of values in subs and pubs. 0:uniform
    double alpha;       // Parameter for Zipf distribution.
    double width;       // Width of a predicate.
    int emax;
    ifstream ifileStream;
    ifileStream.open("./paras.txt");
    ifileStream >> subs >> pubs >> atts >> emax;
    ifileStream.close();
    pubs = atoi(argv[1]) * 1000000;
    vector<double> insertTimeList;
    vector<double> matchTimeList;
    vector<double> matchPubList;

    // Initiate generator
    intervalGenerator gen(subs, pubs);
    gen.ReadSubList();
    gen.ReadPubList();
    


    cout<<"read data finish"<<endl;
    Beq a(atts, emax);

    // insert
    for (int i = 0; i < pubs; i++)
    {
        
        Timer subStart;

        a.insert(gen.pubList[i], gen.pubList);                       // Insert sub[i] into data structure.

        int64_t insertTime = subStart.elapsed_nano();   // Record inserting time in nanosecond.
        insertTimeList.push_back((double) insertTime / 1000000);
        
        
    }


    for (int i = 0; i < subs; i++)
    {

        
        int matchPubs = 0;                              // Record the number of matched subscriptions.
        Timer matchStart;

        a.match(gen.subList[i], matchPubs, gen.pubList);

        int64_t eventTime = matchStart.elapsed_nano();  // Record matching time in nanosecond.
        matchTimeList.push_back((double) eventTime / 1000000);
        matchPubList.push_back(matchPubs);
        
        
        if((i+1)%(subs/10)==0){
            cout<<i+1<<endl;
        }
        
        
    }


    // output
    string outputFileName = "beq.txt";
    string content = Util::Int2String(pubs) + "\t" + 
                    Util::Double2String(Util::Average(insertTimeList)) + "\t" +
                    Util::Double2String(Util::Average(matchTimeList)) + "\t" +
                    Util::Double2String(Util::Variance(matchTimeList)) + "\t" +
                    Util::Double2String(Util::Max(matchTimeList)) + "\t" +
                    Util::Double2String(Util::Min(matchTimeList)) + "\t" +
                    Util::Double2String(Util::Average(matchPubList));
    Util::WriteData(outputFileName.c_str(), content);



    return 0;
}
