#ifndef DNN_H
#define DNN_H

#include <initializer_list>
#include <vector>
#include <utility>
#include <thread>
#include <atomic>
#include <tensorflow/cc/ops/standard_ops.h>
#include <tensorflow/cc/framework/gradients.h>
#include <tensorflow/core/framework/tensor.h>
#include <tensorflow/core/framework/tensor_shape.h>
#include <tensorflow/cc/client/client_session.h>

using namespace std;
using namespace tensorflow;
using namespace tensorflow::ops;


using     ilong   = initializer_list<long>;
using     batch   = pair<vector<vector<double>>,vector<vector<double>>>;


class dnn {

public:
    long         Num_Inputs;
    vector<long> Hidden_Units;
    long         Num_Classes;
    int threshold, trainp, lasttrainp, windowsize, batch_size, epoch;
    bool is_ready_to_train;
    double learning_rate;
    atomic<bool> is_ready_to_predict, thread_stop_flag;
    string modelname;
    Scope R;
    ClientSession* Sess;
    Placeholder      Inp;
    Placeholder      Expected;

    vector<Variable> Weights;
    vector<Variable> Biases;
    vector<Variable> Adam_Ms_b;
    vector<Variable> Adam_Ms;
    vector<Variable> Adam_Vs_b;
    vector<Variable> Adam_Vs;
    vector<Relu>     Hiddens;

    Variable*        Out_Weight;
    Variable*        Out_Bias;
    Variable*        Out_Adam_M_b;
    Variable*        Out_Adam_M;
    Variable*        Out_Adam_V_b;
    Variable*        Out_Adam_V;
    Identity*        Out;

    Relu*            Probs;
    Mean*            Loss;
    vector<ApplyAdam>  Optims;
    vector<long> Indices;
    int IndicesIndex;
    bool firstIndices;

    vector<vector<double>> All_Inps;
    vector<vector<double>> All_Expecteds;

    Tensor Inps;
    Tensor Expecteds;


    dnn(long Num_Inputs,ilong Hidden_Units,long Num_Classes, const Scope &upperscope, ClientSession* uppersess, double learning_rate, int threshold, string modelname);
    ~dnn();


    void init_weights_and_biases();
    void create_hidden_layers();
    void create_output_layer();
    void complete_model();

    void          add_training_data(const vector<double> &Input,const double Label);
    batch         get_rand_batch(long Size);
    void          set_batch(const batch &&Batch);
    void          trainonce();
    void          trainthread();
    void          train(int epoch, int batch_size);
    double         get_current_loss();
    vector<double> infer(const vector<double> &Sample);
};

#endif
