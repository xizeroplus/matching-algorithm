#include <iostream>
#include "opIndex.h"
#include "origin_opIndex.h"
#include <algorithm>
using namespace std;
int main(int argc, char **argv)
{
    int subs;           // Number of subscriptions.
    int pubs;           // Number of publications.
    int attDom;           // Total number of attributes, i.e. dimensions.
    int k;           // Number of constraints(predicates) in one sub.
    int m;              // Number of constraints in one pub.
    int valDom;         // Cardinality of values.
	// -------------------distribution-----------------
    int subAttDis;         // The distribution of attributes in subs and pubs. 0:uniform distribution | 1:Zipf distribution
    int subValDis;         // The distribution of values in subs and pubs. 0:uniform
    int pubAttDis;         // The distribution of attributes in subs and pubs. 0:uniform distribution | 1:Zipf distribution
    int pubValDis;         // The distribution of values in subs and pubs. 0:uniform | 1:Zipf distribution    now using
    double subAttalpha;       // Parameter for Zipf distribution.
    double pubAttalpha;
    double subValalpha;
    double pubValalpha;
	//----------other 7 no use in current.-------------
    double width;       // Width of a predicate. no use in current.
    double equalRatio;   //no use in current.
    int old_seg;
    int new_seg;
    int old_sig;
    int new_sig;

    // read paras
    ifstream parafile;
    parafile.open(string("./paras/paras_")+string(argv[1])+string(".txt"));
    parafile >> subs >> pubs >> k >> m >> subAttDis >> subValDis >> pubAttDis >> pubValDis >> attDom >> valDom
             >> subAttalpha >>subValalpha >>  pubAttalpha >> pubValalpha >> width >> equalRatio>>old_seg >> old_sig>>new_seg>>new_sig;
    parafile.close();
    // define test vectors
    vector<double> insertTimeList;
    vector<double> matchTimeList;
    vector<double> matchSubList;
    vector<int> matchDetailSub;
    vector<double> matchDetailPub;
	vector<double> matchstatustime[5];
	//vector<double> detailTime[6];
    ofstream fileStream;
	const double percentscale[5] = {0.05,0.25,0.5,0.75,0.95};
	int percentindex;

    srand(time(NULL));
    // Initiate generator
	intervalGenerator gen(subs, pubs, k, m, subAttDis, subValDis, pubAttDis, pubValDis, attDom, valDom, subAttalpha ,subValalpha ,pubAttalpha, pubValalpha, width, equalRatio);
    gen.ReadSubList();
    gen.ReadPubList();
    cout << "read datalist finished" << endl;

    // initiate opindex
    origin_opIndex oldop(old_seg,old_sig);
    oldop.calcFrequency(gen.subList);
    opIndex newop(new_seg,new_sig);
    newop.calcFrequency(gen.subList);

    //origin_opIndex

    // insert
    for (int i = 0; i < subs; i++)
    {
        Timer subStart;

        oldop.insert(gen.subList[i]);                       // Insert sub[i] into data structure.

        int64_t insertTime = subStart.elapsed_nano();   // Record inserting time in nanosecond.
        insertTimeList.push_back((double) insertTime / 1000000);
    }

    cout << "insert origin_opIndex finished" << endl;

    // match

    for (int i = 0; i < pubs; i++)
    {
        if (!((i+1)%100)) cout << "matching " << i+1 << "..." << endl;
        int matchSubs = 0;                              // Record the number of matched subscriptions.
        matchDetailPub.clear();
        //matchDetailSub.clear();
        Timer matchStart;
        oldop.match(gen.pubList[i], matchSubs, gen.subList, matchDetailPub, matchDetailSub);
        int64_t eventTime = matchStart.elapsed_nano();  // Record matching time in nanosecond.
		double tmptime=(double) eventTime / 1000000;
        matchTimeList.push_back(tmptime);
        matchSubList.push_back(matchSubs);
		if(!matchSubs) continue;
		for(int i=0;i<5;++i){
			percentindex = (int) floor(matchSubs * percentscale[i]);
			matchstatustime[i].push_back(matchDetailPub[percentindex]);
		}

    }
    cout << "matching origin_opIndex finished" << endl;



    // output
    string outputFileName = "./Detail/origin_opIndex.txt";
    string content = Util::Int2String(subs) + "\t" + Util::Int2String(pubs) + "\t" + Util::Double2String(Util::Average(insertTimeList)) + "\t";
	
                     content+=Util::Double2String(Util::Average(matchTimeList)) + "\t" +
                     Util::Double2String(Util::Average(matchSubList))+ "\t";
	for(int i=0;i<5;++i)content+=Util::Double2String(Util::Average(matchstatustime[i])) + "\t";
    Util::WriteData(outputFileName.c_str(), content);

 
////////////////////////////////////////////////////new op////////////////////////////////////////////
    insertTimeList.clear();
    matchTimeList.clear();
    matchSubList.clear();
	for(int i=0;i<5;++i)matchstatustime[i].clear();
    matchDetailPub.clear();
    matchDetailSub.clear();
    //new_opindex

    // insert
    for (int i = 0; i < subs; i++)
    {
        Timer subStart;

        newop.insert(gen.subList[i]);                       // Insert sub[i] into data structure.

        int64_t insertTime = subStart.elapsed_nano();   // Record inserting time in nanosecond.
        insertTimeList.push_back((double) insertTime / 1000000);
    }

    cout << "insert new_opIndex finished" << endl;

    // match


    for (int i = 0; i < pubs; i++)
    {
        int matchSubs = 0;                              // Record the number of matched subscriptions.
        matchDetailPub.clear();
        matchDetailSub.clear();

        Timer matchStart;
        newop.match(gen.pubList[i], matchSubs, gen.subList, matchDetailPub, matchDetailSub);

        int64_t eventTime = matchStart.elapsed_nano();  // Record matching time in nanosecond.
		double tmptime=(double) eventTime / 1000000;
        matchTimeList.push_back(tmptime);
        matchSubList.push_back(matchSubs);
		if(!matchSubs) continue;
		for(int i=0;i<5;++i){
			percentindex = (int) floor(matchSubs * percentscale[i]);
			matchstatustime[i].push_back(matchDetailPub[percentindex]);
		}

		 
		if (!((i+1)%50)){
			cout << "matching " << i+1 << "..." << endl;
			newop.change();
			cout << "change complete"<<endl;
		}
    }
    cout << "matching new_opIndex finished" << endl;


    // output
    outputFileName = "./Detail/new_opindex.txt";
    content = Util::Int2String(subs) + "\t" + Util::Int2String(pubs) + "\t" + Util::Double2String(Util::Average(insertTimeList)) + "\t";
              content+=Util::Double2String(Util::Average(matchTimeList)) + "\t" +
              Util::Double2String(Util::Average(matchSubList))+ "\t";
	for(int i=0;i<5;++i)content+=Util::Double2String(Util::Average(matchstatustime[i])) + "\t";
    Util::WriteData(outputFileName.c_str(), content);

	
	
    return 0;
}
