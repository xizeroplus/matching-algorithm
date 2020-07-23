#include "dnn.h"

using namespace std;

using ilong   = initializer_list<long>;
using batch   = pair<vector<vector<double>>,vector<vector<double>>>; //pair of input batch & expected batch


dnn::dnn(long Num_Inputs,ilong Hidden_Units,long Num_Classes, const Scope &upperscope, ClientSession* uppersess, double learning_rate, int threshold, string modelname):R(upperscope),Sess(uppersess),firstIndices(true),Inp(Placeholder(R, DT_DOUBLE)),Expected(Placeholder(R, DT_DOUBLE)),All_Inps(threshold),All_Expecteds(threshold),modelname(modelname){
    this->Num_Inputs   = Num_Inputs;
    this->Hidden_Units = Hidden_Units;
    this->Num_Classes  = Num_Classes;
    this->learning_rate = learning_rate;
    this->threshold = threshold;
    this->trainp = 0;
    this->lasttrainp = threshold;
    this->windowsize = threshold;
    this->is_ready_to_train = false;
    this->is_ready_to_predict = false;
    this->thread_stop_flag = false;
    this->batch_size = 100;
    this->epoch = 1000;
}

dnn::~dnn(){
    delete this->Out_Weight;
    delete this->Out_Adam_M;
    delete this->Out_Adam_V;
    delete this->Out_Bias;
    delete this->Out_Adam_M_b;
    delete this->Out_Adam_V_b;
    delete this->Out;
    delete this->Probs;
    delete this->Loss;
}


void dnn::init_weights_and_biases(){

    long Num_Hiddens = this->Hidden_Units.size();
    for (long I=0; I<=Num_Hiddens; I++){      
        long Num_Units = this->Hidden_Units[I];        
        if (I==0){
            this->Weights.push_back(Variable(R, {Num_Inputs,Num_Units}, DT_DOUBLE));
            this->Adam_Ms.push_back(Variable(R, {Num_Inputs,Num_Units}, DT_DOUBLE));
            this->Adam_Vs.push_back(Variable(R, {Num_Inputs,Num_Units}, DT_DOUBLE));
        }
        else
        if (I<Num_Hiddens){
            long Prev_Num_Units = this->Hidden_Units[I-1];
            this->Weights.push_back(Variable(R, {Prev_Num_Units,Num_Units}, DT_DOUBLE));
            this->Adam_Ms.push_back(Variable(R, {Prev_Num_Units,Num_Units}, DT_DOUBLE));
            this->Adam_Vs.push_back(Variable(R, {Prev_Num_Units,Num_Units}, DT_DOUBLE));
        }
        else{
            long Last_Hidden_Size = this->Hidden_Units[Num_Hiddens-1];
            this->Out_Weight      = new Variable(R, {Last_Hidden_Size,Num_Classes}, DT_DOUBLE);
            this->Out_Adam_M      = new Variable(R, {Last_Hidden_Size,Num_Classes}, DT_DOUBLE);
            this->Out_Adam_V      = new Variable(R, {Last_Hidden_Size,Num_Classes}, DT_DOUBLE);
        }      

        if (I<Num_Hiddens){
            this->Biases.push_back(Variable(R, {Num_Units}, DT_DOUBLE));
            this->Adam_Ms_b.push_back(Variable(R, {Num_Units}, DT_DOUBLE));
            this->Adam_Vs_b.push_back(Variable(R, {Num_Units}, DT_DOUBLE));
        }
        else{
            this->Out_Bias = new Variable(R, {Num_Classes}, DT_DOUBLE);        
            this->Out_Adam_M_b = new Variable(R, {Num_Classes}, DT_DOUBLE);        
            this->Out_Adam_V_b = new Variable(R, {Num_Classes}, DT_DOUBLE); 
        }     
    }

    for (long I=0; I<=Num_Hiddens; I++){
        long Num_Units = this->Hidden_Units[I];
        Assign* Weight_Init;
        Assign* Bias_Init;
        Assign* Adam_M_Init;
        Assign* Adam_V_Init;
        Assign* Adam_Mb_Init;
        Assign* Adam_Vb_Init;
        if (I==0){
            Weight_Init  = new Assign(R, this->Weights[I], RandomNormal(R, {(int)Num_Inputs,(int)Num_Units}, DT_DOUBLE));
            Adam_M_Init  = new Assign(R, this->Adam_Ms[I],Input::Initializer(0.0, {(int)Num_Inputs,(int)Num_Units}));
            Adam_V_Init  = new Assign(R, this->Adam_Vs[I],Input::Initializer(0.0, {(int)Num_Inputs,(int)Num_Units}));
            Bias_Init    = new Assign(R, this->Biases[I], RandomNormal(R, {(int)Num_Units}, DT_DOUBLE));
            Adam_Mb_Init = new Assign(R, this->Adam_Ms_b[I],Input::Initializer(0.0, {(int)Num_Units}));
            Adam_Vb_Init = new Assign(R, this->Adam_Vs_b[I],Input::Initializer(0.0, {(int)Num_Units}));
        }
        else if (I<Num_Hiddens) {
            long Prev_Num_Units = this->Hidden_Units[I-1];
            Weight_Init         = new Assign(R, this->Weights[I], RandomNormal(R, {(int)Prev_Num_Units,(int)Num_Units}, DT_DOUBLE));
            Adam_M_Init         = new Assign(R, this->Adam_Ms[I],Input::Initializer(0.0, {(int)Prev_Num_Units,(int)Num_Units}));
            Adam_V_Init         = new Assign(R, this->Adam_Vs[I],Input::Initializer(0.0, {(int)Prev_Num_Units,(int)Num_Units}));
            Bias_Init           = new Assign(R, this->Biases[I], RandomNormal(R, {(int)Num_Units}, DT_DOUBLE));
            Adam_Mb_Init        = new Assign(R, this->Adam_Ms_b[I],Input::Initializer(0.0, {(int)Num_Units}));
            Adam_Vb_Init        = new Assign(R, this->Adam_Vs_b[I],Input::Initializer(0.0, {(int)Num_Units}));
        }
        else {
            long Prev_Num_Units = this->Hidden_Units[Num_Hiddens-1];
            Weight_Init         = new Assign(R, *this->Out_Weight, RandomNormal(R, {(int)Prev_Num_Units,(int)Num_Classes}, DT_DOUBLE));
            Adam_M_Init         = new Assign(R, *this->Out_Adam_M,Input::Initializer(0.0, {(int)Prev_Num_Units,(int)Num_Classes}));
            Adam_V_Init         = new Assign(R, *this->Out_Adam_V,Input::Initializer(0.0, {(int)Prev_Num_Units,(int)Num_Classes}));
            Bias_Init           = new Assign(R, *this->Out_Bias, RandomNormal(R, {(int)Num_Classes}, DT_DOUBLE));
            Adam_Mb_Init        = new Assign(R, *this->Out_Adam_M_b,Input::Initializer(0.0, {(int)Num_Classes}));
            Adam_Vb_Init        = new Assign(R, *this->Out_Adam_V_b,Input::Initializer(0.0, {(int)Num_Classes}));
        }
        TF_CHECK_OK(
        Sess->Run({*Weight_Init,*Bias_Init,*Adam_M_Init,*Adam_V_Init,*Adam_Mb_Init,*Adam_Vb_Init},nullptr)        
        );
        delete Weight_Init;
        delete Bias_Init;
    }
}  


void dnn::create_hidden_layers(){
    long Num_Hiddens = this->Hidden_Units.size();

    for (long I=0; I<Num_Hiddens; I++){
        long Num_Units = this->Hidden_Units[I];
        if (I==0){
            Relu Layer = Relu(R, Add(R, MatMul(R,this->Inp, this->Weights[I]), this->Biases[I]));
            this->Hiddens.push_back(Layer);
        }
        else{
            Relu Layer = Relu(R, Add(R, MatMul(R,this->Hiddens[I-1], this->Weights[I]), this->Biases[I]));
            this->Hiddens.push_back(Layer);
        }
    }
}


void dnn::create_output_layer(){
    long Num_Hiddens = this->Hidden_Units.size();
    this->Out = new Identity(R, Add(R, MatMul(R,this->Hiddens[Num_Hiddens-1],*this->Out_Weight), *this->Out_Bias));
}


void dnn::complete_model(){
    this->Probs = new Relu(R, *this->Out);
    this->Loss  = new Mean(R, Square(R, Sub(R, this->Expected, *this->Probs)), {0,1});

    vector<Output> Grad_Outputs;
    vector<Output> Losses; 
    vector<Output> Vars;

    Losses.push_back(*this->Loss);
    long Num_Hiddens = this->Hidden_Units.size();

    for (long I=0; I<Num_Hiddens; I++)
        Vars.push_back(this->Weights[I]);

    Vars.push_back(*this->Out_Weight);

    for (long I=0; I<Num_Hiddens; I++)
        Vars.push_back(this->Biases[I]);

    Vars.push_back(*this->Out_Bias);

    TF_CHECK_OK(
        AddSymbolicGradients(R, Losses, Vars, &Grad_Outputs)
    );


    for (long I=0; I<Num_Hiddens; I++){
        ApplyAdam Optim = ApplyAdam(R, this->Weights[I], this->Adam_Ms[I], this->Adam_Vs[I], Cast(R,0.9,DT_DOUBLE), Cast(R,0.999,DT_DOUBLE), Cast(R,this->learning_rate,DT_DOUBLE), Cast(R,0.9,DT_DOUBLE), Cast(R,0.999,DT_DOUBLE), Cast(R,1e-7,DT_DOUBLE), {Grad_Outputs[I]});
        this->Optims.push_back(Optim);
    }

    ApplyAdam Outw_Optim = ApplyAdam(R, *this->Out_Weight, *this->Out_Adam_M, *this->Out_Adam_V, Cast(R,0.9,DT_DOUBLE), Cast(R,0.999,DT_DOUBLE), Cast(R,this->learning_rate,DT_DOUBLE), Cast(R,0.9,DT_DOUBLE), Cast(R,0.999,DT_DOUBLE), Cast(R,1e-7,DT_DOUBLE), {Grad_Outputs[Num_Hiddens]});
    this->Optims.push_back(Outw_Optim);

    for (long I=0; I<Num_Hiddens; I++){
        ApplyAdam Optim = ApplyAdam(R, this->Biases[I], this->Adam_Ms_b[I], this->Adam_Vs_b[I], Cast(R,0.9,DT_DOUBLE), Cast(R,0.999,DT_DOUBLE), Cast(R,this->learning_rate,DT_DOUBLE), Cast(R,0.9,DT_DOUBLE), Cast(R,0.999,DT_DOUBLE), Cast(R,1e-7,DT_DOUBLE), {Grad_Outputs[Num_Hiddens+1+I]});
        this->Optims.push_back(Optim);
    } 

    ApplyAdam Outb_Optim = ApplyAdam(R, *this->Out_Bias, *this->Out_Adam_M_b, *this->Out_Adam_V_b, Cast(R,0.9,DT_DOUBLE), Cast(R,0.999,DT_DOUBLE), Cast(R,this->learning_rate,DT_DOUBLE), Cast(R,0.9,DT_DOUBLE), Cast(R,0.999,DT_DOUBLE), Cast(R,1e-7,DT_DOUBLE), {Grad_Outputs[2*Num_Hiddens+1]});
    this->Optims.push_back(Outb_Optim);
}


void dnn::add_training_data(const vector<double> &Input, const double Label){
    this->All_Inps[trainp%threshold] = Input;
    this->All_Expecteds[trainp%threshold] = vector<double>(1, Label);
    ++trainp;

    if(trainp == threshold)
        is_ready_to_train = true;
    if(trainp - lasttrainp > windowsize){
        if(!is_ready_to_predict)
            thread_stop_flag = true;
        else {
            thread_stop_flag = false;
            train(epoch,batch_size);
            lasttrainp = trainp;
        }
    }
}


batch dnn::get_rand_batch(long Size){
    int allsize=threshold;
    if(firstIndices){
        for (long I=0; I<allsize; I++)
            Indices.push_back(I);
        IndicesIndex=0;
        firstIndices=false;
    }

    if(!IndicesIndex)
        random_shuffle(Indices.begin(),Indices.end());

    vector<vector<double>> Inps;
    vector<vector<double>> Expecteds;

    for (long I=0; I<Size && IndicesIndex+I<allsize; I++){
        long Index = Indices[IndicesIndex+I];
        vector<double> Inp      = this->All_Inps[Index];
        vector<double> Expected = this->All_Expecteds[Index];
        Inps.push_back(Inp);
        Expecteds.push_back(Expected);
    }
    IndicesIndex+=Size;
    if(IndicesIndex>=allsize)
        IndicesIndex=0;

    batch Pair = {Inps,Expecteds};

    return Pair;
}


void dnn::set_batch(const batch &&Batch){
    long Size = Batch.first.size();

    this->Inps      = Tensor(DT_DOUBLE, TensorShape({(int)Size, (int)this->Num_Inputs}));
    this->Expecteds = Tensor(DT_DOUBLE, TensorShape({(int)Size, (int)this->Num_Classes}));    

    long I=0;
    for (vector<double> const& Inp: Batch.first)
        for (double Value: Inp){
            this->Inps.flat<double>()(I) = Value;
            I++;
        }

    I=0;
    for (vector<double> const& Expected: Batch.second)
        for (double Value: Expected){
            this->Expecteds.flat<double>()(I) = Value;
            I++;
        }
}


void dnn::trainonce() {
    vector<Output> Ops;
    for (long I=0; I<this->Optims.size(); I++)
        Ops.push_back(this->Optims[I]);
    Ops.push_back(*this->Out);
    TF_CHECK_OK(
        this->Sess->Run(
            {{this->Inp,this->Inps},{this->Expected,this->Expecteds}}, 
            Ops,
            nullptr
        )
    );
}

void dnn::trainthread() {
    this->is_ready_to_predict = false;
    int onetime = floor((double)this->threshold/this->batch_size);
    int trainiter = this->epoch*onetime;
    double loss;
    for(int i=0; i<trainiter && !this->thread_stop_flag; ++i){
        this->set_batch(this->get_rand_batch(this->batch_size));
        this->trainonce();
        if((i+1)%(onetime*10)==0){
            loss=this->get_current_loss();
            cout<<modelname<<" training "<<(i+1)/onetime<<" "<<loss<<endl;
        }
    }
    this->is_ready_to_predict = true;
}

void dnn::train(int epoch, int batch_size){
    if(!is_ready_to_train)
        return;
    this->epoch = epoch;
    this->batch_size = batch_size;
    std::thread t(&dnn::trainthread, this);
    t.detach();
}
        
        

double dnn::get_current_loss(){
    vector<Tensor> Outputs;

    TF_CHECK_OK(
        this->Sess->Run(
            {{this->Inp,this->Inps},{this->Expected,this->Expecteds}}, 
            {*this->Loss}, 
            &Outputs
        )
    );
    return Outputs[0].scalar<double>()();
}


vector<double> dnn::infer(const vector<double> &Sample){
    vector<Tensor> Outputs;

    Tensor Infer_Inp = Tensor(DT_DOUBLE, TensorShape({1,(int)this->Num_Inputs}));

    for (long I=0; I<this->Num_Inputs; I++)
        Infer_Inp.flat<double>()(I) = Sample[I];

    TF_CHECK_OK(
        Sess->Run({{this->Inp,Infer_Inp}}, {*this->Probs}, &Outputs)
    );

    vector<double> Probs;

    for (long I=0; I<this->Num_Classes; I++)
        Probs.push_back(Outputs[0].flat<double>()(I));

    return Probs;
}

