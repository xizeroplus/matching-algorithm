#include<iostream>
#include<fstream>
#include"AWBTree.h"

using namespace std;


int main(int argc, char** argv) {
    int subs;     // Number of subscriptions.
    int pubs;     // Number of publications.
    int atts;     // Total number of attributes, i.e. dimensions.
    int cons;     // Number of constraints(predicates) in one sub.
    int m;        // Number of constraints in one pub.
    int attDis;   // The distribution of attributes in subs and pubs. 0:uniform distribution | 1:Zipf distribution
    int valDis;   // The distribution of values in subs and pubs. 0:uniform, 1: both zipf, -1: sub zipf but pub uniform
    int valDom;   // Cardinality of values.
    double alpha; // Parameter for Zipf distribution.
    double width; // Width of a predicate.
    float Ppoint = 0.25;//partition point
    unsigned short WCsize = 40; //width cell size

    freopen("paras.txt", "r", stdin);
    cin >> subs >> pubs >> atts >> cons >> m >> attDis >> valDis >> valDom;
    cin >> alpha >> width;

    vector<double> insertTimeList;
    vector<double> matchTimeList;
    vector<double> deleteTimeList;
    vector<double> matchSubList;

    // Initiate generator
	
	string name = "/home/customer/Lzhy/data/sub-" + to_string(subs / 1000) + "K-" + to_string(atts) + "D-" + to_string(cons) + "A-" + to_string(attDis) + "Ad-"
	            + to_string(valDis) + "Vd-" + to_string((int)(alpha*10)) + "al-" + to_string((int)(width*10)) + "W-r-5.txt";
    string pname = "/home/customer/Lzhy/data/pub-"+ to_string(subs / 1000) + "K-" + to_string(atts) + "D-" + to_string(m) + "A-" + to_string(attDis) + "Ad-"
	            + to_string(valDis) + "Vd-" + to_string((int)(alpha*10)) + "al-r-5.txt";
    
    intervalGenerator gen(subs, pubs);	
    gen.InsertSubs(name);
    gen.InsertPubs(pname);
	
    atts = gen.atts; valDom = gen.valDom; width = gen.width; attDis = gen.attDis; valDis = gen.valDis;
    cout<<atts<<" "<< gen.m <<" "<<gen.cons<<endl;	
    AWBTree a(atts, valDom, WCsize, Ppoint);

    //insert PobaTree
    cout << "insert start...\n";
    
    for (int i = 0; i < subs; i++)
    {
        Timer subStart;
        a.insert(gen.subList[i]); // Insert sub[i] into data structure.
        int64_t insertTime = subStart.elapsed_nano(); // Record inserting time in nanosecond.
        insertTimeList.push_back((double)insertTime / 1000000);
    }
   
    cout << "insert complete!\n";

    // match
    for (int i = 0; i < pubs; i++)
    {
        int matchSubs = 0;                              // Record the number of matched subscriptions.
        Timer matchStart;

        //a.forward(gen.pubList[i], matchSubs, gen.subList, WCsize);
		//a.forward_opt(gen.pubList[i], matchSubs, gen.subList, WCsize);
		//a.forward_a(gen.pubList[i], matchSubs, gen.subList, WCsize);
		//a.backward(gen.pubList[i], matchSubs, gen.subList, 0);
        //a.backward_opt(gen.pubList[i], matchSubs, gen.subList, 0);
        a.hybrid_opt(gen.pubList[i], matchSubs, gen.subList, Ppoint);
		//a.hybrid_a(gen.pubList[i], matchSubs, gen.subList, Ppoint);

        int64_t eventTime = matchStart.elapsed_nano();  // Record matching time in nanosecond.
        matchTimeList.push_back((double)eventTime / 1000000);
        matchSubList.push_back(matchSubs);

        //cout << matchSubs << endl;
    }
    cout << "match complete!\n";
	
    double memCost = a.mem_size();
    cout << "mem cost: " << memCost / (1024 * 1024) << "MB\n";

    // delete
    	
    int step = subs / 1000;
    for (int i = 0; i < subs; i += step)
    {
        Timer subStart;

        a.deleteSub(gen.subList[i]); // Delete sub[i] into data structure.

        int64_t deleteTime = subStart.elapsed_nano(); // Record inserting time in nanosecond.
        deleteTimeList.push_back((double)deleteTime / 1000000);
    }
    
    
    // output
	/*
    string outputFileName = "sabatree-sub-w.txt";
    string content = Util::Int2String(subs) + "\t" + Util::Double2String(Util::Average(insertTimeList)) + "\t" +
        Util::Double2String(Util::Average(deleteTimeList)) + "\t" +
        Util::Double2String(Util::Average(matchTimeList)) + "\t" +
        Util::Double2String(Util::Average(matchSubList)) + "\t" +
        Util::Double2String(Util::ComputeDoubleStatistics(matchTimeList)[1]) + "\t" +
        Util::Double2String(Util::ComputeDoubleStatistics(matchTimeList)[2]) + "\t" +
        Util::Double2String(Util::ComputeDoubleStatistics(matchTimeList)[3]);
    Util::WriteData(outputFileName.c_str(), content);
	*/
    //}
    //Util::Check(gen, "awb");
	
	string outputFileName = "match_time.txt";
	ofstream outputfile(outputFileName, ios::app);
    if (!outputfile) {
        cout << "error opening destination file." << endl;
        return 0;
    }
    for (int i = 0;i < pubs; i++) {
        outputfile << matchSubList[i] << " " << matchTimeList[i] << "\n";
    }
    outputfile.close();
	
    return 0;
}
