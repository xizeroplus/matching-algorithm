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
    int sleepms;
    int autowindow = 0;

    ifstream parafile;
    parafile.open(string("./paras/paras_")+string(argv[1])+string(".txt"));
    parafile >> subs >> pubs >> atts >> cons >> m >> valDom >> width >> buck_num >> tama_level >> learning_rate >> threshold >> epoch >> batch_size >> sleepms >> autowindow;
    parafile.close();
    m = atts;
    vector<double> insertTimeList;
    vector<double> matchTimeList;
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
    decision deci_model(atts,{128},1,learning_rate,threshold,atts,valDom,autowindow,10000);
    deci_model.init();
    cout<<"deci_model init complete"<<endl;
    
    // insert
    for (int i = 0; i < subs; i++)
    {
        Timer start;
        rein.insert(gen.subList[i]);
        tama.insert(gen.subList[i]);
        deci_model.insertSub(gen.subList[i]);
        insertTimeList.push_back((double)start.elapsed_nano()/1000000);
    }
    cout << "insert complete"<<endl;

    bool trained = false;

    // match
    for (int i = 0; i < pubs; i++)
    {
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
        if(result){
            tama.match(pub, matchSubs, gen.subList);
            double eventTime = (double)matchStart.elapsed_nano() / 1000000;
            matchTimeList.push_back(eventTime+deciTime);
            matchSubList.push_back(matchSubs);
            deci_model.tama.add_training_data(samplex,eventTime);
            deci_model.insertPub(gen.pubList[i], eventTime, 1);
            deci_model.feedback(eventTime);
            cout<<"match using tama="<<eventTime<<" match num="<<matchSubs<<' '<<deci_model.tama.trainp<<endl;
        }
        else{
            rein.match(pub, matchSubs);
            double eventTime = (double)matchStart.elapsed_nano() / 1000000;
            matchTimeList.push_back(eventTime+deciTime);
            matchSubList.push_back(matchSubs);
            deci_model.rein.add_training_data(samplex,eventTime);
            deci_model.insertPub(gen.pubList[i], eventTime, 0);
            deci_model.feedback(eventTime);
            cout<<"match using rein="<<eventTime<<" match num="<<matchSubs<<' '<<deci_model.rein.trainp<<endl;
        }
        
        if(!trained && deci_model.is_ready_to_train()){
                deci_model.train(epoch,batch_size);
                trained = true;
        }
        
        usleep(sleepms*1000);

    }
    
    content = Util::Int2String(autowindow) + "\t" +
              Util::Double2String(Util::Average(matchTimeList)) + "\t" +
              Util::Double2String(Util::Variance(matchTimeList)) + "\t" +
              Util::Double2String(Util::Average(deciTimeList)) + "\t" +
              Util::Double2String(Util::Variance(deciTimeList)) + "\t" +
              Util::Double2String(Util::Average(matchSubList));
    Util::WriteData(outputFileName.c_str(), content);

    return 0;
}
