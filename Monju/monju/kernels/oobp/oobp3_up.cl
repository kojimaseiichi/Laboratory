/*
 # SUMMARY
    This kernel is to compute the beysian stochastic propagation that is performed through the network by OOBP algorithm.

 # WORK ITEM(THREADING PLAN)
    [0] threads for every units in the upper basis
    [1] multiplicity of parallel execution for one node in the upper basis
 # WORK GROUP(RESOURCE SHARING PLAN)
    [0] 1
    [1] same as the work item

 # TEMPLATE ARGUMENTS
    X:  number of nodes in the upper basis
    Y:  number of nodes in the lower basis
    XU: number of units per node in the upper basis
    YU: number of units per node in the lower basis
    MP: max number of multiplication of parallel executions

 # PARAMETERS
    ig_jsize:       sizes of each entries in the jagged array
    ig_joff         offset to the first element of the column-oriented entry
    ig_jadj         offset to the first element of the sparse adjacensy matrix
    ig_jlen         length of whole the jag array which representates the CPT sparse matrix
    ig_y_lambda     LAMBDA variable in the lower basis
    ig_y_pi         PI variable in the lower basis
    ig_w_cpt        the CPT matrix that intervenes between two bases
    ig_w_kappa      KAPPA variable that intervenes between two bases and is near the lower basis
    og_w_lambda     LAMBDA variable that intervenes between two bases and is near the upper basis
    og_x_lambda     LAMBDA variable in the upper basis

*/

#define MAX_PARALLEL_SIZE 16

__kernel void oobp3_up_X${X}_Y${Y}_XU${XU}_YU${YU}_MP${MP}(
    __global int* ig_jsize,
    __global int* ig_joff,
    __global int* ig_jadj,
    __global int ig_jlen,
    __global float* ig_y_lambda,
    __global float* ig_y_pi,
    __global float* ig_w_cpt,
    __global float* ig_w_kappa,
    __global float* og_w_lambda,
    __global float* og_x_lambda
)
{
    const size_t kUpperNodeIndex = get_global_id(0);
    const size_t kParallelIndex = get_global_id(1);
    const size_t kParallelSize = get_global_size(1);
    const int CPT_CELL_SIZE = ${XU} * ${YU};    // size of CPT matrix (rows * cols)

    const int jent_size   = ig_jsize[kUpperNodeIndex]; // length of entry
    const int jent_offset = ig_joff[kUpperNodeIndex];  // 
    __global int* g_jadj = ig_jadj + jent_offset;

    // "lo_x_lambda" is an local accumulation for INTER-BASIS LAMBDA over all nodes in the lower basis
    __local float lo_x_lambda[${XU} * ${MP}];
    __local float* lo_x_lambda_offset = lo_x_lambda + ${XU} * (kParallelIndex);
    for (int n = 0; n < ${XU}; n ++)
        lo_x_lambda_offset[n] = 1.0f;

    // repeat a partial calculation of the upward propagation over all nodes that is adjacent with current node in the upper basis
    // then accumulating the local outcome of LABMDA to "lo_x_lambda"
    for (int i = 0; i < jent_size; i ++)
    {
        const int adj = g_jadj[i]; // set an adjacent node in the lower basis
        // below are declarations of the temporal variables
        float y_lambda[${YU}];
        float y_pi[${YU}];
        float w_cpt[CPT_CELL_SIZE];
        float w_kappa[${YU}];
        float w_lambda[${XU}];
        // loading lambda, pi and kappa from the global memory space to the registry
        __global float* g_y_lambda = ig_y_lambda + (${YU} * adj);
        __global float* g_y_pi = ig_y_pi + (${YU} * ajd);
        __global float* g_w_kappa = ig_w_kappa + ${YU} * (jent_offset + i);
        for (int n = 0; n < ${YU}; n ++)
        {
            y_lambda = g_y_lambda[n];
            y_pi = g_y_pi[n];
            w_kappa = g_w_kappa[n];
        }
        // loading the CPT matrix from the global memory space to the registry
        __global float* g_w_cpt = ig_w_cpt + CPT_CELL_SIZE * (jent_offset + i);
        for (int n = 0; n < CPT_CELL_SIZE; n ++)
            w_cpt[n] = g_w_cpt[n];
        // performing an acutual calculation of the propagation
        float sum_outer = 0.0f; // using for a standardization to let mean of LAMBDA come to zero
        for (int col = 0; col < ${XU}; col ++)
        {
            float* cpt_col_vec = w_cpt + col * ${YU};
            float sum = 0.0f;
            // sum(lambda*(pi-kappa+w))
            for (int row = 0; row < ${YU}; row ++)
                sum += y_lambda[row] * (y_pi[row] - w_kappa[row] + cpt_col_vec[row]);
            sum_outer += sum;
            w_lambda[col] = sum;
        }
        // to calculate a intermediate outcome
        // then write back the outcomes
        __global float* g_w_lambda = og_w_lambda + ${XU} * (jent_offset + i);
        for (int col = 0; col < ${XU}; col ++)
        {
            float la = w_lambda[col] / sum_outer;
            lo_x_lambda_offset[col] *= la;
            g_w_lambda[col] = la;
        }
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    // to accumulate concerned LAMBDAs then write back the final outcome to the global memory
#if ${MP} = 1
    __global float* g_x_lambda = og_x_lambda + ${XU} * kUpperNodeIndex;
    for (int n = 0; n < ${XU}; n ++)
        g_x_lambda[n] = lo_x_lambda[n];
#else
    if (kParallelIndex == 0)
    {
        __global float* g_x_lambda = og_x_lambda + ${XU} * kUpperNodeIndex;
        float acc_lambda[${XU}];
        for (int n = 0; n < ${XU}; n ++)
            acc_lambda[n] = lo_x_lambda[n];
        for (int k = 1; k < kParallelSize; k ++)
        {
            __local float* lo_acc_lambda = lo_x_lambda + (${XU} * k);
            for (int n = 0; n < ${XU}; n ++)
                acc_lambda[n] *= lo_acc_lambda[n];
        }
        for (int n = 0; n < ${XU}; n ++)
            g_x_lambda[n] = acc_lambda[n];
    }
#endif
}