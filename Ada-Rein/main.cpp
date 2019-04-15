#include <iostream>
#include "rein.h"
using namespace std;
int main(int argc, char **argv)
{
    int subs, pubs, atts, cons, m, valDom;
    int attDis;     // 0:uniform  1:zipf
    int valDis;     // 0:uniform
    double alpha, width;
    srand(unsigned(time(NULL)));
    subs = Util::String2Int(argv[1]);
    freopen("paras.txt","r",stdin);
    cin >> pubs >> atts >> cons >> m >> attDis >> valDis >> valDom;
    cin >> alpha >> width;
    m = atts;       // Note that Rein requires m == atts
    vector<double> insertTimeList;
    vector<double> matchTimeList;
    vector<double> matchSubList;
    vector<double> preciseMatchTimeList;
    vector<double> preciseMatchSubList;

    intervalGenerator gen(subs, pubs, atts, cons, m, attDis, valDis, valDom, alpha, width);

    int testNum = 1;
    while(testNum -- )
    {
        Rein a(valDom);
        gen.GenSubList();

        for (int i = 0; i < subs; i++)
        {
            Timer subStart;

            a.insert(gen.subList[i]);

            int64_t subTime = subStart.elapsed_nano();
            insertTimeList.push_back((double) subTime / 1000000);
        }
        //cout << "insertion done" << endl;

        a.select_skipped_atts(gen.subList, atts, 0.035);
        gen.GenPubList();


        // Perform regular matching as Rein 
        for (int i = 0; i < pubs; i++)
        {
            int matchSubs = 0;
            int64_t eventTime = a.match(gen.pubList[i], matchSubs, gen.subList);
            preciseMatchTimeList.push_back((double) eventTime / 1000000);
            preciseMatchSubList.push_back(matchSubs);
        }


        // Perform Ada-Rein matching 
        for (int i = 0; i < pubs; i++)
        {
            int matchSubs = 0;

            int64_t eventTime = a.approx_match(gen.pubList[i], matchSubs, gen.subList);

            matchTimeList.push_back((double) eventTime / 1000000);
            matchSubList.push_back(matchSubs);
        }



    }
    string fileName = "ada-rein.txt";
    vector<double> statistics1 = Util::ComputeDoubleStatistics(matchTimeList);
    vector<double> statistics2 = Util::ComputeDoubleStatistics(preciseMatchTimeList);
    string content = Util::Int2String(subs) + "\t" + Util::Double2String(Util::mean(insertTimeList)) + "\t" + Util::Double2String(Util::mean(matchTimeList)) + "\t" + Util::Double2String(Util::mean(matchSubList)) + "\t" + Util::Double2String(Util::mean(preciseMatchTimeList)) + "\t" + Util::Double2String(Util::mean(preciseMatchSubList)) + "\t";
    content += Util::Double2String(100 * (1 - Util::mean(matchTimeList) / Util::mean(preciseMatchTimeList))) + "%\t" + Util::Double2String(100 * (Util::mean(matchSubList) - Util::mean(preciseMatchSubList)) / Util::mean(matchSubList)) + "%";
    content += Util::Double2String(statistics1[1]) + "\t" + Util::Double2String(statistics1[2]) + "\t" + Util::Double2String(statistics1[3]) + "\t" + Util::Double2String(statistics2[1]) + "\t" + Util::Double2String(statistics2[2]) + "\t" + Util::Double2String(statistics2[3]);

    Util::WriteData(fileName.c_str(),content);

    //Util::check(gen, "frein");

    return 0;
}
