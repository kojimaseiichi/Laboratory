/*
 # ATTENTION!
    You can write this source only with Latin-1 character set. DO NOT use Japanese characters.

 # SUMMARY
    This kernel is to drive a sort of bayesian network.
    The algorithm performs a propagating stochastic variables through the the network that is a full-connected bipartile graph.
    You may need to call oobp3_full_up_2() to obtain LAMBDA from "og_w_lambda" in the basis after this kernel execution done

 # WORK ITEM
    [0] threads for every nodes in the lower basis (number of rows)
    [1] threads for every nodes in the upper basis (number of columns)

 # WORK GROUP
    [0] 1
    [1] same as the work item


 # TEMPLATE ARGUMENTS
    X:  number of nodes in the upper basis
    Y:  number of nodes in the lower basis
    XU: number of units per node in the upper basis
    YU: number of units per node in the lower basis

 # PARAMETERS
    ig_y_lambda     LAMBDA in the lower basis
    ig_y_pi         PI in the lower basis
    ig_w_cpt        the CPT matrix that intervenes between two bases
    ig_w_kappa      KAPPA that intervenes between two bases and is near the lower basis
    og_w_lambda     LAMBDA that intervenes between two bases and is near the upper basis

 */
__kernel void oobp3_full_up_1_X${X}_Y${Y}_XU${XU}_YU${YU}(
    __global float* ig_y_lambda,
    __global float* ig_y_pi,
    __global float* ig_w_cpt,
    __global float* ig_w_kappa,
    __global float* og_w_lambda
)
{
    const float kEpsilon = 0.0001;
    const size_t kWIRow = get_global_id(0);
    const size_t kWICol = get_global_id(1);
    const size_t kWGRow = get_group_id(0);
    const size_t kWGCol = get_group_id(1);
    const size_t kCptSize = ${XU} * ${YU};
    const size_t kLinearCptId = kWICol * ${Y} + kWIRow;

    // to load data shared among threads in the same work-group
    __local float lo_y_lambda[${YU}];
    __local float lo_y_pi[${YU}];
    if (kWICol == 0)
    {
        __global float* g_y_lambda_offset = ig_y_lambda + (kWIRow * ${YU});
        __global float* g_y_pi_offset = ig_y_pi + (kWIRow * ${YU});
        for (int n = 0; n < ${YU}; n ++)
        {
            lo_y_lambda[n] = g_y_lambda_offset[n];
            lo_y_pi[n] = g_y_pi_offset[n];
        }
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    // to perform algorithm for every work items
    float w_lambda[${XU}];  // attention! this variable is in the upper basis
    __global float* g_w_cpt_current = ig_w_cpt + kLinearCptId * kCptSize;
    __global float* g_w_kappa_current = ig_w_kappa + kLinearCptId * ${YU};
    float reciprocal_beta = 0.0f;
    for (int j = 0; j < ${XU}; j ++)
    {
        __global float* g_w_cpt_col_vec = g_w_cpt_current + ${YU} * j;
        float sum = 0.0f;
        for (int i = 0; i < ${YU}; i ++)
            sum += lo_y_lambda[i] * (lo_y_pi[i] - g_w_kappa_current[i] + g_w_cpt_col_vec[i]);
        w_lambda[j] = sum;
        reciprocal_beta += sum;
    }
    // to calculate the final outcome then write back the outcome to global memory
    __global float* og_w_lambda_current = og_w_lambda + kLinearCptId * ${XU};
    for (int j = 0; j < ${XU}; j ++)
        og_w_lambda_current[j] = w_lambda[j] / reciprocal_beta;

}