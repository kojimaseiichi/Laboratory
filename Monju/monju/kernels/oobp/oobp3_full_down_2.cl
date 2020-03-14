/*
 # ATTENTION!
    You can write this source only with Latin-1 character set. DO NOT use Japanese characters.

 # SUMMARY
    This kernel is called posterior from function "oobp_full_down_1"

 # WORK ITEM
    [0] threads for every nodes in the lower basis (number of rows)
    [1] 1

 # WORK GROUP
    N/A

 # TEMPLATE ARGUMENTS
    X:  number of nodes in the upper basis
    Y:  number of nodes in the lower basis
    XU: number of units per node in the upper basis
    YU: number of units per node in the lower basis

 # PARAMETERS
    ig_w_kappa      KAPPA variable that intervenes between two bases and is near the upper basis
    og_y_pi         PI variable in the lower basis

 */
__kernel void oobp3_full_down_2_X${X}_Y${Y}_XU${XU}_YU${YU}(
    __global float* ig_w_kappa,
    __global float* og_y_pi
)
{
    const size_t kWIRow = get_global_id(0);

    __global float* g_w_kappa_offset = ig_w_kappa + (kWIRow) * ${YU};
    const int kStrideKappa = ${Y} * ${YU};
   
    // initialize pi
    float sumPi[${YU}];
    for (int n = 0; n < ${YU}; n ++)
        sumPi[n] = 0.0f;
    // accumulate kappa
    for (int col = 0; col < ${X}; col ++)
    {
        for (int n = 0; n < ${YU}; n ++)
            sumPi[n] += g_w_kappa_offset[n];
        g_w_kappa_offset += kStrideKappa;
    }
    // return outcome
    __global float* g_y_pi_offset = og_y_pi + kWIRow * ${YU};
    for (int n = 0; n < ${YU}; n ++)
        g_y_pi_offset[n] = sumPi[n];
}