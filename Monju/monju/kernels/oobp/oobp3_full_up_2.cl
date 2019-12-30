/*
 # SUMMARY
    This kernel is called posterior from function "oobp_full_up_1"

 # WORK ITEM
    [0] 1
    [1] threads for every units in the upper basis (number of columns)

 # WORK GROUP
    N/A

 # TEMPLATE ARGUMENTS
    X:  number of nodes in the upper basis
    Y:  number of nodes in the lower basis
    XU: number of units per node in the upper basis
    YU: number of units per node in the lower basis

 # PARAMETERS
    ig_w_lambda     LAMBDA variable that intervenes between two bases and is near the upper basis
    og_x_lambda     LAMBDA variable in the upper basis

 */
__kernel oobp3_full_up_2_X${X}_Y${Y}_XU${XU}_YU${YU}(
    __global float* ig_w_lambda,
    __global float* og_x_lambda
)
{
    const size_t kWICol = get_global_id(1);

    __global float* g_w_lambda_offset = ig_w_lambda + (${Y} * ${XU} * kWICol);
    float sum[${XU}];
    for (int n = 0; n < ${XU}; n ++)
        sum[n] = 1.0f;
    for (int i = 0; i < ${Y}; i ++)
    {
        prefetch(g_w_lambda_offset, ${XU});
        for (int n = 0; n < ${XU}; n ++)
            sum[n] *= g_w_lambda_offset[n];
        g_w_lambda_offset += ${XU};
    }

    __global float* g_x_lambda_offset = og_x_lambda + kWICol * ${XU};
    for (int n = 0; n < ${XU}; n ++)
        g_x_lambda_offset[n] = sum[n];
}