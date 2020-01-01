/*
 # SUMMARY
    This kernel is to compute the beysian stochastic propagation that is performed through the full-connected network by OOBP algorithm.
    You may need to call oobp3_full_down_2() to obtain LAMBDA from "og_w_kappa" in the basis after this kernel execution done

 # WORK ITEM
    [0] threads for every units in the lower basis (number of rows)
    [1] threads for every units in the upper basis (number of columns)
    
 # WORK GROUP
    [0] same as the work item
    [1] 1

 # TEMPLATE ARGUMENTS
    X:  number of nodes in the upper basis
    Y:  number of nodes in the lower basis
    XU: number of units per node in the upper basis
    YU: number of units per node in the lower basis

 # PARAMETERS
    ig_x_rho        RHO variable in the upper basis
    ig_w_cpt        the CPT matrix that intervenes between two bases
    ig_w_lambda     LAMBDA variable that intervenes between two bases and is near the upper basis
    og_w_kappa      KAPPA variable that intervenes between two bases and is near the lower basis

*/
__kernel void oobp3_full_down_1_X${X}_Y${Y}_XU${XU}_YU${YU}(
    __global float* ig_x_rho,
    __global float* ig_w_cpt,
    __global float* ig_w_lambda,
    __global float* og_w_kappa
)
{
    const size_t kWIRow = get_global_id(0);
    const size_t kWICol = get_global_id(1);
    const size_t kWGRow = get_group_id(0);
    const size_t kWGCol = get_group_id(1);
    const size_t kCptSize = ${XU} * ${YU};

    // to load variable that is shared among the work group
    __local float lo_u_rho[${XU}];
    if (kWIRow == 0)
    {
        __global float* g_x_rho_offset = ig_x_rho + (kWICol * ${XU});
        for (int j = 0; j < ${XU}; j ++)
        {
            lo_u_rho[j] = g_x_rho_offset[j];
        }
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    __global float* g_w_cpt_offset = ig_w_cpt + (kWIRow + kWICol * ${Y}) * kCptSize;
    __global float* g_w_lambda_offset = ig_w_lambda + (kWIRow + kWICol * ${Y}) * ${XU};
    __global float* g_w_kappa_offset = og_w_kappa + (kWIRow + kWICol * ${Y}) * ${YU};

    // to calculate the standardized sumation of "rho(x) / lambda_y(x)"
    float w_pi[${XU}];
    float sum = 0.0f;
    prefetch(g_w_lambda_offset, ${XU});
    for (int j = 0; j < ${XU}; j ++)
    {
        float a = lo_u_rho[j] / g_w_lambda_offset[j];
        sum += a;
        w_pi[j] = a;
    }
    for (int j = 0; j < ${XU}; j ++)
    {
        w_pi[j] /= sum;
    }

    // to calculate the sumation of "cpt_xy(y,x) * pi_y(x)"
    float w_kappa[${YU}];
    for (int i = 0; i < ${YU}; i ++)
    {
        w_kappa[i] = 0.0f;
    }

    for (int j = 0; j < ${XU}; j ++)
    {
        float pi = w_pi[j];
        __global float* g_w_cpt_offset_col = g_w_cpt_offset + j * ${YU};
        for (int i = 0; i < ${YU}; i ++)
            w_kappa[i] += pi * g_w_cpt_offset_col[i];
    }
    
    // to store the outcome
    for (int i = 0; i < ${YU}; i ++)
        g_w_kappa_offset[i] = w_kappa[i];
}
