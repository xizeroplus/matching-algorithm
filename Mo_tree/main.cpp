#include <iostream>
#include "motree.h"
using namespace std;
int main(int argc, char **argv)
{
    int subs;           // Number of subscriptions.
    int pubs;           // Number of publications.
    int valDom;         // Cardinality of values.
    int layer;			// layer of Motree, including 2 lng&lat layer and layer-2 normal atts
	int level;			// level of Motree in each layer, having 1+2+3+4+... cells

	ifstream ifileStream;
	ifileStream.open("./paras.txt");
    ifileStream >> subs >> pubs >> valDom >> layer >> level;
    ifileStream.close();

    vector<double> insertSubTimeList;
	vector<double> insertPubTimeList;
    vector<double> matchSubTimeList;
	vector<double> matchPubTimeList;
    vector<double> matchSubList;
	vector<double> matchPubList;

    // Initiate generator
    intervalGenerator gen(subs, pubs);
    gen.ReadSubList();
    gen.ReadPubList();

	cout<<"load data complete"<<endl;
    Motree a(layer, level, valDom);
	cout<<"start"<<endl;

    for (int i = 0; i < subs; i++)
    {
		//if(i%(subs/10)==0)cout<<i<<endl;
		int matchPubs = 0;
		int64_t insertTime = 0, matchTime = 0;
		
		

        a.insertSub(gen.subList[i], matchPubs, insertTime, matchTime, gen.pubList);
        //int64_t eventTime = matchStart.elapsed_nano();
        insertSubTimeList.push_back((double) insertTime / 1000000);
		//matchPubTimeList.push_back((double) matchTime / 1000000);
        //matchPubList.push_back(matchPubs);
		
	}
	cout<<"insert sub complete"<<endl;
	for (int i = 0; i < pubs; i++)
    {	
		int matchSubs = 0;
		int64_t insertTime = 0, matchTime = 0;

        
		a.insertPub(gen.pubList[i], matchSubs, insertTime, matchTime, gen.subList);
		//int64_t eventTime = matchStart.elapsed_nano();
        //insertPubTimeList.push_back((double) insertTime / 1000000);
		matchSubTimeList.push_back((double) matchTime / 1000000);
		matchSubList.push_back(matchSubs);
		
		if((i+1)%(pubs/10)==0){
			cout<<i+1<<endl;
		}
		
    }
	cout<<"complete"<<endl;


	
	
	
	


    // output
    string outputFileName = "motree.txt";
    string content = Util::Int2String(subs) + "\t" +
					Util::Double2String(Util::Average(insertSubTimeList)) + "\t" +
                    Util::Double2String(Util::Average(matchSubTimeList)) + "\t" +
					Util::Double2String(Util::Variance(matchSubTimeList)) + "\t" +
					Util::Double2String(Util::Max(matchSubTimeList)) + "\t" +
					Util::Double2String(Util::Min(matchSubTimeList)) + "\t" +
                    Util::Double2String(Util::Average(matchSubList)) + "\t";
    Util::WriteData(outputFileName.c_str(), content);




    return 0;
}
