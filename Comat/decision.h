#ifndef DECISION_H
#define DECISION_H

#include <algorithm>
#include "dnn.h"
#include "data_structure.h"
#include "util.h"


class decision{

public:
    long         Num_Inputs;
    vector<long> Hidden_Units;
    long         Num_Classes;
    Scope R,reinscope,tamascope;
    ClientSession Sess;
    int threshold, attrbutes, valDom, buckets, bucketstep, totalstat, windowcount,windowtotal,statp[2];
    bool autowindow;
    double splitpoint;
    vector<vector<double>> statarr, statlabel, stattraindata;
    vector<double> predictresult;
    dnn rein,tama;

    decision(long Num_Inputs,ilong Hidden_Units,long Num_Classes, double learning_rate, int threshold, int attrbutes, int valDom, bool autowindow, int buckets);
    ~decision();

    

    void init();
    void insertSub(const IntervalSub &sub);
    void insertPub(const Pub &pub, double time, int tama);
    void train(int epoch, int batch_size);

    bool is_ready_to_train();
    bool is_ready_to_predict();
    void trainstat();
    int decide(const Pub &pub);


    void infer(const vector<double> &Sample);
    void feedback(double realtime);
    double computePos(const Pub &pub);
};

#endif
