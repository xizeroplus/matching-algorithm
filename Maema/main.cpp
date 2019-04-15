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
    int testNum = 1;
    while(testNum -- )
    {
        Rein a(valDom);
        intervalGenerator gen(subs, pubs, atts, cons, m, attDis, valDis, valDom, alpha, width);
        gen.GenSubList();

        for (int i = 0; i < subs; i++)
        {
            Timer subStart;
            a.insert(gen.subList[i]);

            int64_t subTime = subStart.elapsed_nano();
            insertTimeList.push_back((double) subTime / 1000000);
        }
        //cout << "insertion done" << endl;

        gen.GenPubList();

        for (int i = 0; i < pubs; i++)
        {
            int matchSubs = 0;
            int x = 3 * 500 * (1 - pow((1 - width) / (1 - pow(width, cons)), 1.0 / (cons - 1))) + 15;

            int64_t eventTime = a.match(gen.pubList[i], matchSubs, gen.subList, x);

            //int64_t eventTime = matchStart.elapsed_nano();
            matchTimeList.push_back((double) eventTime / 1000000);
            matchSubList.push_back(matchSubs);
        }
    }
    string fileName = "Maema.txt";
    string content = Util::Int2String(subs) + "\t" + Util::Double2String(Util::mean(insertTimeList)) + "\t" + Util::Double2String(Util::mean(matchTimeList)) + "\t" + Util::Double2String(Util::mean(matchSubList));
    Util::WriteData(fileName.c_str(),content);

    return 0;
}
