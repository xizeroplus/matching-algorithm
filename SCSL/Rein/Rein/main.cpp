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
	// -------------------distribution-----------------
    int subAttDis;         // The distribution of attributes in subs and pubs. 0:uniform distribution | 1:Zipf distribution
    int subValDis;         // The distribution of values in subs and pubs. 0:uniform
    int pubAttDis;         // The distribution of attributes in subs and pubs. 0:uniform distribution | 1:Zipf distribution
    int pubValDis;         // The distribution of values in subs and pubs. 0:uniform | 1:Zipf distribution   now using
    double subAttalpha;       // Parameter for Zipf distribution.
    double pubAttalpha;
    double subValalpha;
    double pubValalpha;  
	//----------other 7 no use in current.-------------
    double width;       // Width of a predicate. no use in current.
    double equalRatio;  //no use in current.
    int old_buck_num;
    int new_buck_num;
	double limitscale; //adjust limit
	int newlevel; //number of levels
	int cstep; //adjustment time window
    ifstream parafile;
    parafile.open(string("./paras/paras_")+string(argv[1])+string(".txt"));
    parafile >> subs >> pubs >> k >> m >> subAttDis >> subValDis >> pubAttDis >> pubValDis >> attDom >> valDom >> subAttalpha >>subValalpha >>  pubAttalpha >> pubValalpha >> width >> equalRatio >> old_buck_num >> new_buck_num>>limitscale>>newlevel>>cstep;
    parafile.close();
	const double percentscale[5] = {0.05,0.25,0.5,0.75,0.95};
    //m = atts;           // Note that Rein requires m == atts.
    vector<double> insertTimeList;
    vector<double> matchTimeList;
    vector<double> matchSubList;
    vector<int> matchDetailSub;
    vector<double> matchDetailPub;
    vector<double> matchDetailTime[5];


    // Initiate generator
    intervalGenerator gen(subs, pubs, k, m, subAttDis, subValDis, pubAttDis, pubValDis, attDom, valDom, subAttalpha ,subValalpha ,pubAttalpha, pubValalpha, width, equalRatio);
    gen.ReadSubList();
    gen.ReadPubList();
    cout << "read datalist finished" << endl;

    oldRein old_Rein(valDom, old_buck_num);
	Rein new_Rein(valDom,10, new_buck_num, limitscale, newlevel);

	ofstream fileStream;
	int percentindex;
	string windowcontent="";
	bool firstchange=true;
	double matchingtime = 0;
	
	/////////////////////////////////start old rein///////////////////////////////////////////
    // insert
    for (int i = 0; i < subs; i++)
    {
        Timer subStart;

        old_Rein.insert(gen.subList[i]);                       // Insert sub[i] into data structure.

        int64_t insertTime = subStart.elapsed_nano();   // Record inserting time in nanosecond.
        insertTimeList.push_back((double) insertTime / 1000000);
    }

	cout << "insert old_Rein finished" << endl;

    // match
    for (int i = 0; i < pubs; i++)
    {
        if (!((i+1)%100)) cout << "matching " << i+1 << "..." << endl;
        int matchSubs = 0;                              // Record the number of matched subscriptions.
        matchDetailPub.clear();
        matchDetailSub.clear();
        Timer matchStart;

        old_Rein.match(gen.pubList[i], matchSubs, gen.subList, matchDetailPub);

        int64_t eventTime = matchStart.elapsed_nano();  // Record matching time in nanosecond.
        matchTimeList.push_back((double) eventTime / 1000000);
        matchSubList.push_back(matchSubs);
		for(int i=0;i<5;++i){
			percentindex = (int) floor(matchSubs * percentscale[i]);
			matchDetailTime[i].push_back(matchDetailPub[percentindex]);
		}

          
    }

    cout << "matching old_Rein finished" << endl;


    // output
    string outputFileName = "./Detail/old_rein.txt";
    string content = Util::Int2String(subs) + "\t" + Util::Double2String(Util::Average(insertTimeList)) + "\t" +
                     Util::Double2String(Util::Average(matchTimeList)) + "\t" +
                     Util::Double2String(Util::Average(matchSubList)) + "\t";
    for(int i=0;i<5;++i) content+=Util::Double2String(Util::Average(matchDetailTime[i]))+"\t";
    Util::WriteData(outputFileName.c_str(), content);


	
  ////////////////////////////////////start new rein//////////////////////////////////////////////////


	
    insertTimeList.clear();
    matchTimeList.clear();
    matchSubList.clear();
	firstchange=true;
    matchDetailPub.clear();
    matchDetailSub.clear();
	matchingtime = 0;
	for(int i=0;i<5;++i) matchDetailTime[i].clear();
	windowcontent="";
    // insert
	for (int i = 0; i < subs; i++)
    {
        //Timer subStart;
		int64_t insertTime;
        new_Rein.insert(gen.subList[i],insertTime);                       // Insert sub[i] into data structure.

        //int64_t insertTime = subStart.elapsed_nano();   // Record inserting time in nanosecond.
        insertTimeList.push_back((double) insertTime / 1000000);
    }
	
    cout << "insert new_Rein finished" << endl;
	//new_Rein100.check();
    // match
    for (int i = 0; i < pubs; i++)
    {
        
        int matchSubs = 0;                              // Record the number of matched subscriptions.
        matchDetailPub.clear();
        matchDetailSub.clear();
		
        Timer matchStart;
		int64_t eventTime;
        new_Rein.match(gen.pubList[i], matchSubs, gen.subList, matchDetailPub);

        eventTime = matchStart.elapsed_nano();  // Record matching time in nanosecond.
        matchTimeList.push_back((double) eventTime / 1000000);
        matchSubList.push_back(matchSubs);
		for(int i=0;i<5;++i){
			percentindex = (int) floor(matchSubs * percentscale[i]);
			matchDetailTime[i].push_back(matchDetailPub[percentindex]);
		}

		if (!((i+1)%cstep)){
			cout << "matching " << i+1 << "..." << endl;
			if(i+1==pubs)break;
				if(firstchange){
					matchingtime = Util::Average(matchTimeList);
					firstchange=false;
				}
			Timer changeStart;
			int changenum=new_Rein.change(gen.subList,cstep,matchingtime,windowcontent);
			int64_t changeTime = changeStart.elapsed_nano();
			matchTimeList[i]+=(double) changeTime / 1000000;
			cout<<"change complete "<<changenum<<endl;
			//new_Rein100.check();
		}
    }

    cout << "matching new_Rein finished" << endl;

	

    // output
    outputFileName = "./Detail/new_Rein.txt";
    content = Util::Int2String(new_buck_num) + "\t" + Util::Double2String(Util::Average(insertTimeList))+ "\t";
                     content+=Util::Double2String(Util::Average(matchTimeList)) + "\t" +
                     Util::Double2String(Util::Average(matchSubList))+ "\t";
    for(int i=0;i<5;++i) content+=Util::Double2String(Util::Average(matchDetailTime[i]))+"\t";
    Util::WriteData(outputFileName.c_str(), content);
	
	
	outputFileName = "./Detail/detail.txt";
	Util::WriteData(outputFileName.c_str(), windowcontent);
	

	

    return 0;
}
