このファイルは要らない
/*
 # ATTENTION!
    You can write this source only with Latin-1 character set. DO NOT use Japanese characters.

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
    ig_x_r          R variable in the upper basis
    og_x_lambda     LAMBDA variable in the upper basis

 */
__kernel void oobp3_full_up_3_X${X}_Y${Y}_XU${XU}_YU${YU}(
    __global float* ig_x_r,
    __global float* og_x_lambda
)
{
    const size_t kWICol = get_global_id(1);

    __global float* g_x_r_offset = ig_x_r + kWICol * ${XU};
    __global float* g_x_lambda_offset = og_x_lambda + kWICol * ${XU};

    for (int j = 0; j < ${XU}; j ++)
    {
        g_x_lambda_offset[j] *= g_x_r_offset;
    }
}
