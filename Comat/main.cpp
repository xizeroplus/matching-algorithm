#include <iostream>
#include <utility>
#include <unistd.h>
#include "generator.h"
#include "rein.h"
#include "tama.h"
#include "decision.h"
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
    double learning_rate;
    int threshold;
    int epoch;
    int batch_size;
    ifstream parafile;
    parafile.open(string("./paras/paras_")+string(argv[1])+string(".txt"));
    parafile >> subs >> pubs >> atts >> cons >> m >> valDom >> width >> buck_num >> tama_level >> learning_rate >> threshold >> epoch >> batch_size;
    parafile.close();

    m = atts;
    vector<double> comatTimeList;
    vector<double> reinTimeList;
    vector<double> tamaTimeList;
    vector<double> deciTimeList;
    vector<int> matchSubList;
    // Initiate generator
    intervalGenerator gen(subs, pubs, atts, cons, m, valDom, width);
    gen.ReadSubList();
    gen.ReadPubList();
    cout<<"read complete"<<endl;
    string outputFileName = "./result/comat.txt";
    string content = "";
    Rein rein(valDom, buck_num);
    Tama tama(atts, valDom, tama_level);
    decision deci_model(atts,{128},1,learning_rate,threshold,atts,valDom,false,10000);
    deci_model.init();
    cout<<"deci_model init complete"<<endl;
    
    // insert
    for (int i = 0; i < subs; i++)
    {
        rein.insert(gen.subList[i]);
        tama.insert(gen.subList[i]);
        deci_model.insertSub(gen.subList[i]);
    }
    cout << "insert complete"<<endl;

    bool trained = false;
    
    for(int i=0; i<threshold; ++i){
        const Pub &pub = gen.pubList[i];
        vector<double> samplex(atts);
         for(int i=0; i<atts; ++i)
            samplex[pub.pairs[i].att]=(double)pub.pairs[i].value / valDom;
        
        int matchSubs = 0;

        Timer matchStart;
        tama.match(pub, matchSubs, gen.subList);
        double eventTime = (double)matchStart.elapsed_nano() / 1000000;
        tamaTimeList.push_back(eventTime);
        deci_model.tama.add_training_data(samplex,eventTime);
        matchStart.reset();
        rein.match(pub, matchSubs);
        eventTime = (double)matchStart.elapsed_nano() / 1000000;
        reinTimeList.push_back(eventTime);
        deci_model.rein.add_training_data(samplex,eventTime);

    }
    deci_model.tama.trainthread();
    double tamaloss = deci_model.tama.get_current_loss();
    if(tamaloss > Util::Average(tamaTimeList)) //loss too high, sth wrong during training
        return 1;
    deci_model.rein.trainthread();
    double reinloss = deci_model.rein.get_current_loss();
    if(reinloss > Util::Average(reinTimeList))
        return 1;
    int reintime=0,tamatime=0;
    tamaTimeList.clear();
    reinTimeList.clear();
    for (int i = threshold; i < pubs; i++){
        if((i+1)%100==0)
            cout << "matching " << i+1 << "..." << endl;
        const Pub &pub = gen.pubList[i];
        int matchSubs = 0;
        Timer start;
        int result = deci_model.decide(pub);
        double deciTime = (double)start.elapsed_nano() / 1000000;
        deciTimeList.push_back((double)start.elapsed_nano()/1000000);
        vector<double> samplex(atts);
        for(int i=0; i<atts; ++i)
            samplex[pub.pairs[i].att]=(double)pub.pairs[i].value / valDom;
            
        Timer matchStart;
        tama.match(pub, matchSubs, gen.subList);
        double tamaTime = (double)matchStart.elapsed_nano() / 1000000;
        tamaTimeList.push_back(tamaTime);
        matchStart.reset();
        rein.match(pub, matchSubs);
        double reinTime = (double)matchStart.elapsed_nano() / 1000000;
        reinTimeList.push_back(reinTime);
        if(result){
            comatTimeList.push_back(tamaTime+deciTime);
            tamatime++;
        }
        else{
            comatTimeList.push_back(reinTime+deciTime);
                reintime++;
        }       

    }
    
    content =   string(argv[1]) + "\t" +
                Util::Double2String(Util::Average(comatTimeList)) + "\t" +
                Util::Double2String(Util::Variance(comatTimeList)) + "\t" +
                Util::Double2String(Util::Average(deciTimeList)) + "\t" +
                Util::Double2String(Util::Variance(deciTimeList)) + "\t" +
                Util::Int2String(reintime) + "\t" +
                Util::Int2String(tamatime);
                Util::WriteData(outputFileName.c_str(), content);

    content =   string(argv[1]) + "\t" +
                Util::Double2String(Util::Average(reinTimeList)) + "\t" +
                Util::Double2String(Util::Variance(reinTimeList));
    outputFileName = "./result/rein.txt";
    Util::WriteData(outputFileName.c_str(), content);

    content =   string(argv[1]) + "\t" +
                Util::Double2String(Util::Average(tamaTimeList)) + "\t" +
                Util::Double2String(Util::Variance(tamaTimeList));
    outputFileName = "./result/tama.txt";
    Util::WriteData(outputFileName.c_str(), content);
    
    return 0;
}
