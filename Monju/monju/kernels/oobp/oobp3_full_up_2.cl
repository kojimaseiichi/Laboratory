/*
 # ATTENTION!
    You can write this source only with Latin-1 character set. DO NOT use Japanese characters.

 # SUMMARY
    This kernel is called posterior from function "oobp_full_up_1"

 # WORK ITEM
    [0] 1
    [1] threads for every nodes in the upper basis (number of columns)

 # WORK GROUP
    N/A

 # TEMPLATE ARGUMENTS
    X:  number of nodes in the upper basis
    Y:  number of nodes in the lower basis
    XU: number of units per node in the upper basis
    YU: number of units per node in the lower basis

 # PARAMETERS
    ig_w_lambda     LAMBDA variable that intervenes between two bases and is near the upper basis
    ig_x_r          RESTRICTION variable that regularizes the model
    og_x_lambda     LAMBDA variable in the upper basis

 */
__kernel void oobp3_full_up_2_X${X}_Y${Y}_XU${XU}_YU${YU}(
    __global float* ig_w_lambda,
    __global float* ig_x_r,
    __global float* og_x_lambda
)
{
    const size_t kWICol = get_global_id(1);
    const size_t kLinearCptId = kWICol * ${Y};

    // performe the algorithm
    __global float* g_w_lambda_group = ig_w_lambda + kLinearCptId * ${XU};
    float prod_lambda[${XU}];
    for (int n = 0; n < ${XU}; n ++)
        prod_lambda[n] = 1.0f;
    for (int i = 0; i < ${Y}; i ++)
    {
        for (int n = 0; n < ${XU}; n ++)
            prod_lambda[n] *= g_w_lambda_group[n];
        g_w_lambda_group += ${XU};  // next lambda
    }

    // write back the outcome
    __global float* g_x_lambda_current = og_x_lambda + kWICol * ${XU};
    __global float* g_x_r_current = ig_x_r + kWICol * ${XU};
    for (int n = 0; n < ${XU}; n ++)
        g_x_lambda_current[n] = prod_lambda[n] * g_x_r_current[n];
}