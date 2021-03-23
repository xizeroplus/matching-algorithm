#include <iostream>
#include <fstream>
#include "PobaTree.h"

using namespace std;
int main(int argc, char **argv)
{
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

    size_t degree = 2000;

    freopen("paras.txt", "r", stdin);
    cin >> subs >> pubs >> atts >> cons >> m >> attDis >> valDis >> valDom;
    cin >> alpha >> width;

    m = atts; // Note that PobaTree requires m == atts.
    vector<double> insertTimeList;
    vector<double> matchTimeList;
    vector<double> deleteTimeList;
    vector<double> matchSubList;

    // Initiate generator
    intervalGenerator gen(subs, pubs, atts, cons, m, attDis, valDis, valDom, alpha, width);
    gen.GenSubList();
    gen.GenPubList();
    vector<IntervalSub> &allSubs = gen.subList;
    vector<Pub> &allPubs = gen.pubList;

    char buf[1024];
    // snprintf(buf, 128, "subsFile/subs_%d_%d_%d_%d_%d_%d_%d_%d_%f_%f",
    //          subs, pubs, atts, cons, m, attDis, valDis, valDom, alpha, width);
    // fstream subFile(buf, std::ios::in);
    // vector<IntervalSub> allSubs;
    // allSubs.reserve(subs);
    // while (subFile)
    // {
    //     IntervalSub sub;
    //     subFile >> sub.id >> sub.size;
    //     for (int i = 0; i < sub.size; i++)
    //     {
    //         IntervalCnt cnt;
    //         subFile >> cnt.att >> cnt.lowValue >> cnt.highValue;
    //         sub.constraints.push_back(cnt);
    //     }
    //     allSubs.push_back(sub);
    // }
    // subFile.close();

    // snprintf(buf, 128, "pubsFile/pubs_%d_%d_%d_%d_%d_%d_%d_%d_%f_%f",
    //          subs, pubs, atts, cons, m, attDis, valDis, valDom, alpha, width);
    // fstream pubFile(buf, std::ios::in);
    // vector<Pub> allPubs;
    // allPubs.reserve(pubs);
    // while (pubFile)
    // {
    //     Pub pub;
    //     pubFile >> pub.size;
    //     for (int i = 0; i < pub.size; i++)
    //     {
    //         Pair cur_pair;
    //         pubFile >> cur_pair.att >> cur_pair.value;
    //         pub.pairs.push_back(cur_pair);
    //     }
    //     allPubs.push_back(pub);
    // }
    // pubFile.close();

    PobaTree a(atts, degree);

    for (int i = 0; i < subs; i++)
    {
        Timer subStart;

        a.insert(allSubs[i]); // Insert sub[i] into data structure.

        int64_t insertTime = subStart.elapsed_nano(); // Record inserting time in nanosecond.
        insertTimeList.push_back((double)insertTime / 1000000);
    }

    cout << "PobaTree insert finished\n";

    // match
    for (int i = 0; i < pubs; i++) //note
    {
        int matchSubs = 0; // Record the number of matched subscriptions.
        Timer matchStart;

        a.match(allPubs[i], matchSubs, allSubs);
        // a.match2(allPubs[i], matchSubs, allSubs);

        int64_t eventTime = matchStart.elapsed_nano(); // Record matching time in nanosecond.
        matchTimeList.push_back((double)eventTime / 1000000);
        matchSubList.push_back(matchSubs);
    }
    cout << "PobaTree match finished\n";


    // output
    // string outputFileName = "PobaTree.txt";
    // snprintf(buf, 1024,
    //          "insertAvg=%f, matchAvg=%f, matchNumAvg=%f\t"
    //          "[subs=%d, pubs=%d, atts=%d, constraints=%d, "
    //          "pub_constraints=%d, valueDomian=%d, attrDistribute=%d, "
    //          "valueDistribute=%d, alpha=%f, width=%f]\t\tB+tree degree=%d",
    //          Util::Average(insertTimeList),
    //          Util::Average(matchTimeList), Util::Average(matchSubList),
    //          subs, pubs, atts, cons, m, valDom,
    //          attDis, valDis, alpha, width, degree);
    // Util::WriteData(outputFileName.c_str(), buf);

    // delete
    int step = subs / 10000;
    for (int i = 0; i < subs; i += step)
    {
        Timer subStart;

        a.deleteSub(allSubs[i]); // Insert sub[i] into data structure.

        int64_t deleteTime = subStart.elapsed_nano(); // Record inserting time in nanosecond.
        deleteTimeList.push_back((double)deleteTime / 1000000);
    }

    // size_t mem_size = a.mem_size();
    string outputFileName = "PobaTreeDegree.txt";
    snprintf(buf, 1024,
             "insertAvg=%f, deleteAvg=%f, matchAvg=%f, with total num=%d "
             "B+tree degree=%d",
             Util::Average(insertTimeList),
             Util::Average(deleteTimeList),
             Util::Average(matchTimeList), subs, degree);
    Util::WriteData(outputFileName.c_str(), buf);

    return 0;
}
