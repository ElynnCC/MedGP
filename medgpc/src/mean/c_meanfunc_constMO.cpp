/*
-------------------------------------------------------------------------
This is the function file for top const mean function class.
-------------------------------------------------------------------------
*/
#include <iostream>
#include <vector>
#include <mkl.h>
#include <omp.h>
#include <time.h>
#include "mean/c_meanfunc_constMO.h"
#include "util/global_settings.h"

using namespace std;


c_meanfunc_constMO::c_meanfunc_constMO(){
    meanfunc_name = "c_meanfunc_const";
    meanfunc_hyp_num = 0;
}
c_meanfunc_constMO::c_meanfunc_constMO(vector<int> input_param):c_meanfunc(input_param){
    meanfunc_name = "c_meanfunc_const";
    meanfunc_hyp_num = input_param[1];
}
c_meanfunc_constMO::c_meanfunc_constMO(vector<int> input_param, vector<double> input_hyp):c_meanfunc(input_param, input_hyp){
    meanfunc_name = "c_meanfunc_const";
    meanfunc_hyp_num = input_param[1];

    if(int(input_hyp.size()) != meanfunc_hyp_num){
        cout << "ERROR: mismatch # of hyperparameters! ";
        cout << "Get " << int(input_hyp.size()) << ", but expect " << meanfunc_hyp_num << endl;
        exit(1);
    }
}

void c_meanfunc_constMO::compute_mean_vector(
                                            const vector<int> &meta, 
                                            const vector<float> &x, 
                                            const bool &flag_grad, 
                                            float *&mean_vector, 
                                            vector<float*> &mean_gradients
                                            ){
    int i, dim;
    dim = int(x.size());
    time_t  t1, t2;
    
    // #pragma omp parallel if(GLOBAL_USE_OMP)
    // omp_set_num_threads(GLOBAL_OMP_MEANFUNC_THREAD_NUM);
    // #pragma omp parallel for private(i) firstprivate(dim)
    for(i = 0; i < dim; i++){
        mean_vector[i] = meanfunc_hyp[meta[i]];
    }
}

void c_meanfunc_constMO::compute_mean_gradients(
                                                const vector<int> &meta, 
                                                const vector<float> &x,
                                                const float *chol_alpha,
                                                vector<double> &gradients
                                                ){
    int dim = int(x.size());
    MKL_INT n = dim;
    gradients.clear();
    for(int i = 0; i < meanfunc_hyp_num; i++){
        vector<float> grad_mean(dim, 0.0);
        for(int j = 0; j < dim; j++){
            if(meta[j] == i){
                grad_mean[j] = 1.0;
            }
        }
        double dot = cblas_sdsdot(n, 0.0f, &grad_mean[0], 1, chol_alpha, 1);
        gradients.push_back(dot);
    }
}
