#include <iostream>
#include <utility>
#include <unistd.h>
#include "generator.h"
#include "rein.h"
#include "tama.h"
#include "enmat.h"
#include "enmatp.h"
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
    int buck_num;
    int tama_level;
    int update_window;
    ifstream parafile;
    parafile.open(string("./paras/paras_")+string(argv[1])+string(".txt"));
    parafile >> subs >> pubs >> atts >> cons >> m >> valDom >> width >> buck_num >> tama_level >> update_window;
    parafile.close();

    m = atts;
    vector<double> comatTimeList, reinTimeList, tamaTimeList, enmatTimeList[3], enmatpTimeList[3], enmatprTimeList[3];



    // Initiate generator
    intervalGenerator gen(subs, pubs, atts, cons, m, valDom, width);
    gen.GenSubList();
    gen.GenPubList();
    cout<<"gen complete"<<endl;
    string outputFileName = "";
    string content = "";
    Rein rein(valDom, buck_num);
    Tama tama(atts, valDom, tama_level);
    Enmat enmat(valDom, buck_num, atts, tama_level, false);
    Enmatp enmatp(valDom, buck_num, atts, tama_level, false);
    Enmatp enmatpr(valDom, buck_num, atts, tama_level, true);
    cout<<"init complete"<<endl;
    
    int cursub = 0, curpub = 0;
    const int subwindow = 100, pubwindow = 10;
    while (cursub < subs){
        for (int i = 0; i < subwindow; i++)
        {
            const IntervalSub &sub = gen.subList[cursub+i];
            rein.insert(sub);
            tama.insert(sub);
            enmat.insert(sub);
            enmatp.insert(sub);
            enmatpr.insert(sub);
        }
        cursub += subwindow;
        cout << "insert complete "<< cursub <<endl;

        for (int i = 0; i < pubwindow; ++i){
            const Pub &pub = gen.pubList[i];
            int matchSubs = 0;

            Timer matchStart;
            tama.match(pub, matchSubs, gen.subList);
            double tamaTime = (double)matchStart.elapsed_nano() / 1000000;
            tamaTimeList.push_back(tamaTime);

            matchStart.reset();
            rein.match(pub, matchSubs);
            double reinTime = (double)matchStart.elapsed_nano() / 1000000;
            reinTimeList.push_back(reinTime);
 

            int matchSubs1 = 0, matchSubs2 = 0;
            int64_t t1 = 0, t2 = 0;
            matchStart.reset();
            enmat.match(pub, gen.subList, matchSubs1, matchSubs2, t1, t2);
            double enmatTime = (double)matchStart.elapsed_nano() / 1000000;
            enmatTimeList[0].push_back(enmatTime);
            enmatTimeList[1].push_back((double)t1 / 1000000);
            enmatTimeList[2].push_back((double)t2 / 1000000);

            matchSubs1 = matchSubs2 = 0;
            t1 = t2 = 0;
            matchStart.reset();
            enmatp.match(pub, gen.subList, matchSubs1, matchSubs2, t1, t2);
            double enmatpTime = (double)matchStart.elapsed_nano() / 1000000;
            enmatpTimeList[0].push_back(enmatpTime);
            enmatpTimeList[1].push_back((double)t1 / 1000000);
            enmatpTimeList[2].push_back((double)t2 / 1000000);


            matchSubs1 = matchSubs2 = 0;
            t1 = t2 = 0;
            matchStart.reset();
            enmatpr.match(pub, gen.subList, matchSubs1, matchSubs2, t1, t2);
            double enmatprTime = (double)matchStart.elapsed_nano() / 1000000;
            enmatprTimeList[0].push_back(enmatprTime);
            enmatprTimeList[1].push_back((double)t1 / 1000000);
            enmatprTimeList[2].push_back((double)t2 / 1000000);
        }
        curpub += pubwindow;
        cout << "match complete "<< curpub <<endl;

        if (!(curpub % update_window))
            cout<< "update split point: " << enmatp.adjustsplit() << endl;


        if(!(cursub % 100000)){
                    content =   Util::Int2String(cursub) + "\t" +
                        Util::Double2String(Util::Average(reinTimeList)) + "\t" +
                        Util::Double2String(Util::Variance(reinTimeList));
            outputFileName = "./result/rein.txt";
            Util::WriteData(outputFileName.c_str(), content);

            content =   Util::Int2String(cursub) + "\t" +
                        Util::Double2String(Util::Average(tamaTimeList)) + "\t" +
                        Util::Double2String(Util::Variance(tamaTimeList));
            outputFileName = "./result/tama.txt";
            Util::WriteData(outputFileName.c_str(), content);

            content =   Util::Int2String(cursub) + "\t" +
                        Util::Double2String(Util::Average(enmatTimeList[0])) + "\t" +
                        Util::Double2String(Util::Variance(enmatTimeList[0])) + "\t" +
                        Util::Double2String(Util::Average(enmatTimeList[1])) + "\t" +
                        Util::Double2String(Util::Variance(enmatTimeList[1])) + "\t" +
                        Util::Double2String(Util::Average(enmatTimeList[2])) + "\t" +
                        Util::Double2String(Util::Variance(enmatTimeList[2]));
            outputFileName = "./result/enmat.txt";
            Util::WriteData(outputFileName.c_str(), content);

            content =   Util::Int2String(cursub) + "\t" +
                        Util::Double2String(Util::Average(enmatpTimeList[0])) + "\t" +
                        Util::Double2String(Util::Variance(enmatpTimeList[0])) + "\t" +
                        Util::Double2String(Util::Average(enmatpTimeList[1])) + "\t" +
                        Util::Double2String(Util::Variance(enmatpTimeList[1])) + "\t" +
                        Util::Double2String(Util::Average(enmatpTimeList[2])) + "\t" +
                        Util::Double2String(Util::Variance(enmatpTimeList[2]));
            outputFileName = "./result/enmatp.txt";
            Util::WriteData(outputFileName.c_str(), content);

            content =   Util::Int2String(cursub) + "\t" +
                        Util::Double2String(Util::Average(enmatprTimeList[0])) + "\t" +
                        Util::Double2String(Util::Variance(enmatprTimeList[0])) + "\t" +
                        Util::Double2String(Util::Average(enmatprTimeList[1])) + "\t" +
                        Util::Double2String(Util::Variance(enmatprTimeList[1])) + "\t" +
                        Util::Double2String(Util::Average(enmatprTimeList[2])) + "\t" +
                        Util::Double2String(Util::Variance(enmatprTimeList[2]));
            outputFileName = "./result/enmatpr.txt";
            Util::WriteData(outputFileName.c_str(), content);
            
        }
    }



    // content =   Util::Int2String(atts) + "\t" +
    //             Util::Double2String(Util::Average(reinTimeList)) + "\t" +
    //             Util::Double2String(Util::Variance(reinTimeList));
    // outputFileName = "./result/rein.txt";
    // Util::WriteData(outputFileName.c_str(), content);

    // content =   Util::Int2String(atts) + "\t" +
    //             Util::Double2String(Util::Average(tamaTimeList)) + "\t" +
    //             Util::Double2String(Util::Variance(tamaTimeList));
    // outputFileName = "./result/tama.txt";
    // Util::WriteData(outputFileName.c_str(), content);

    // content =   Util::Int2String(atts) + "\t" +
    //             Util::Double2String(Util::Average(enmatTimeList[0])) + "\t" +
    //             Util::Double2String(Util::Variance(enmatTimeList[0])) + "\t" +
    //             Util::Double2String(Util::Average(enmatTimeList[1])) + "\t" +
    //             Util::Double2String(Util::Variance(enmatTimeList[1])) + "\t" +
    //             Util::Double2String(Util::Average(enmatTimeList[2])) + "\t" +
    //             Util::Double2String(Util::Variance(enmatTimeList[2]));
    // outputFileName = "./result/enmat.txt";
    // Util::WriteData(outputFileName.c_str(), content);

    // content =   Util::Int2String(atts) + "\t" +
    //             Util::Double2String(Util::Average(enmatpTimeList[0])) + "\t" +
    //             Util::Double2String(Util::Variance(enmatpTimeList[0])) + "\t" +
    //             Util::Double2String(Util::Average(enmatpTimeList[1])) + "\t" +
    //             Util::Double2String(Util::Variance(enmatpTimeList[1])) + "\t" +
    //             Util::Double2String(Util::Average(enmatpTimeList[2])) + "\t" +
    //             Util::Double2String(Util::Variance(enmatpTimeList[2]));
    // outputFileName = "./result/enmatp.txt";
    // Util::WriteData(outputFileName.c_str(), content);
    
    return 0;
}
