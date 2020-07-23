#include "decision.h"


static inline int random1(int x)
{
    return (int) (x * (rand() / (RAND_MAX + 1.0)));
}






decision:: decision(long Num_Inputs,ilong Hidden_Units,long Num_Classes, double learning_rate, int threshold, int attrbutes, int valDom, bool autowindow, int buckets):R(Scope::NewRootScope()),reinscope(R.NewSubScope("rein")),tamascope(R.NewSubScope("tama")),Sess(R),rein(Num_Inputs,Hidden_Units,Num_Classes,reinscope,&Sess,learning_rate,threshold,"rein"),tama(Num_Inputs,Hidden_Units,Num_Classes,tamascope,&Sess,learning_rate,threshold,"tama"),statarr(attrbutes,vector<double>(buckets)),statlabel(2,vector<double>(threshold)),stattraindata(2,vector<double>(threshold)),predictresult(2){
    this->Num_Inputs   = Num_Inputs;
    this->Hidden_Units = Hidden_Units;
    this->Num_Classes  = Num_Classes;
    this->threshold = threshold;
    this->attrbutes = attrbutes;
    this->valDom = valDom;
    this->autowindow = autowindow;
    this->buckets = buckets;
    this->bucketstep = valDom / buckets;
    this->totalstat = 0;
    this->splitpoint = 0.5;
    this->windowcount = this->windowtotal = 0;
    statp[0] = statp[1] = 0;
    srand(time(NULL));
};


decision::~decision(){
}

void decision::init(){
    rein.init_weights_and_biases();
    rein.create_hidden_layers();
    rein.create_output_layer();
    rein.complete_model();
    tama.init_weights_and_biases();
    tama.create_hidden_layers();
    tama.create_output_layer();
    tama.complete_model();
}

void decision::insertSub(const IntervalSub &sub){
    for (int i = 0; i < sub.size; i++){
        int start = sub.constraints[i].lowValue / bucketstep;
        int end = sub.constraints[i].highValue / bucketstep;
        for (int j = start; j <= end; ++j)
            ++statarr[sub.constraints[i].att][j];
        ++totalstat;
    }
}
    
void decision::insertPub(const Pub &pub, double time, int tama){//tama:0(rein) or 1(tama)
    stattraindata[tama][statp[tama]] = computePos(pub);
    statlabel[tama][statp[tama]++] = time;
    statp[tama] %= threshold;
    if(is_ready_to_train() && statp[tama]==0){
        trainstat();
    }
}
    
void decision::train(int epoch, int batch_size){
    trainstat();
    rein.train(epoch, batch_size);
    tama.train(epoch, batch_size);
}
    
bool decision::is_ready_to_train(){
    return rein.is_ready_to_train && tama.is_ready_to_train;
}
bool decision::is_ready_to_predict(){
    return rein.is_ready_to_predict && tama.is_ready_to_predict;
}

void decision::trainstat(){
    pair<double, double> reinline = Util::linfit(stattraindata[0],statlabel[0]);
    pair<double, double> tamaline = Util::linfit(stattraindata[1],statlabel[1]);
    splitpoint = (tamaline.second - reinline.second) / (reinline.first - tamaline.first);
}
    
int decision::decide(const Pub &pub){//return 0:rein 1:tama
    if(!is_ready_to_predict()) {
        if(is_ready_to_train()) {
            double pos = computePos(pub);
            if (pos > splitpoint)
                return 0;
            else
                return 1;
        }
        return random1(2);
    }
    vector<double> testx(attrbutes);
    for (int i = 0; i < attrbutes; i++){
        testx[i] = (double)pub.pairs[i].value / valDom;
    }
    infer(testx);
    if(predictresult[0] < predictresult[1]){
        return 0;
    }
    else{
        return 1;
    }
}

void decision::infer(const vector<double> &Sample){
    vector<Tensor> Outputs;
    Tensor Infer_Inp = Tensor(DT_DOUBLE, TensorShape({1,(int)this->Num_Inputs}));
    for (long I=0; I<this->Num_Inputs; I++)
      Infer_Inp.flat<double>()(I) = Sample[I];
    TF_CHECK_OK(
      Sess.Run({{rein.Inp,Infer_Inp},{tama.Inp,Infer_Inp}}, {*rein.Probs,*tama.Probs}, &Outputs)
    );
    predictresult[0] = Outputs[0].flat<double>()(0);
    predictresult[1] = Outputs[1].flat<double>()(0);
}


void decision::feedback(double realtime){
    if(!autowindow || !is_ready_to_predict())
        return;
    if(predictresult[0]+predictresult[1]<realtime*2)
        ++windowcount;
    ++windowtotal;
    if(windowtotal==threshold/40){
        if((double)windowcount/windowtotal>0.5){
            rein.windowsize = (int) max(rein.windowsize/2,threshold/20);
            tama.windowsize = (int) max(tama.windowsize/2,threshold/20);
        }
        else{
            rein.windowsize = (int) min(rein.windowsize*1.1,(double)rein.threshold);
            tama.windowsize = (int) min(tama.windowsize*1.1,(double)tama.threshold);
        }
        windowtotal = windowcount = 0;
    }
}
    
    
double decision::computePos(const Pub &pub){
    int count = 0;
    for (int i = 0; i < pub.size; i++){
        int att = pub.pairs[i].att;
        int buck = pub.pairs[i].value / bucketstep;
        count += statarr[att][buck];
    }
    return (double) count / totalstat;
}
