/*
 # SUMMARY
    This kernel is to compute the beysian stochastic propagation that is performed through the full-connected network by OOBP algorithm.
    You may need to call oobp3_full_up_2() to obtain LAMBDA from "og_w_lambda" in the basis after this kernel execution done

 # WORK ITEM
    [0] threads for every units in the lower basis (number of rows)
    [1] threads for every units in the upper basis (number of columns)

 # WORK GROUP
    [0] 1
    [1] same as the work item


 # TEMPLATE ARGUMENTS
    X:  number of nodes in the upper basis
    Y:  number of nodes in the lower basis
    XU: number of units per node in the upper basis
    YU: number of units per node in the lower basis

 # PARAMETERS
    ig_y_lambda     LAMBDA variable in the lower basis
    ig_y_pi         PI variable in the lower basis
    ig_w_cpt        the CPT matrix that intervenes between two bases
    ig_w_kappa      KAPPA variable that intervenes between two bases and is near the lower basis
    og_w_lambda     LAMBDA variable that intervenes between two bases and is near the upper basis
    og_x_lambda     LAMBDA variable in the upper basis

 */
__kernel oobp3_full_up_1_X${X}_Y${Y}_XU${XU}_YU${YU}(
    __global float* ig_y_lambda,
    __global float* ig_y_pi,
    __global float* ig_w_cpt,
    __global float* ig_w_kappa,
    __global float* og_w_lambda,
    __global float* og_x_lambda
)
{
    const size_t kWIRow = get_global_id(0);
    const size_t kWICol = get_global_id(1);
    const size_t kWGRow = get_group_id(0);
    const size_t kWGCol = get_group_id(1);
    const size_t kCptSize = ${XU} * ${YU};

    // to load data shared among threads in the same work-group
    __local float lo_y_lambda[${YU}];
    __local float lo_y_pi[${YU}];
    if (kWICol == 0)
    {
        __global float* g_y_lambda_offset = ig_y_lambda + (kWIRow * ${YU});
        __global float* g_y_pi_offset = ig_y_pi + (kWIRow * ${YU});
        event_t event_copy = async_work_group_copy(lo_y_lambda, g_y_lambda_offset, ${YU}, 0);
        event_copy = async_work_group_copy(lo_y_pi, g_y_pi_offset, ${YU}, event_copy);
        wait_group_events(1, &event_copy);
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    // to perform algorithm for every work items
    float w_lambda[${XU}];  // attention! this variable that is belonged to the upper basis
    __global float* g_w_cpt_offset = ig_w_cpt + (kWICol * ${Y} + kWIRow) * kCptSize;
    __global float* g_w_kappa_offset = ig_w_kappa + (kWICol * ${Y} + kWIRow) * ${YU};
    float outer_sum = 0.0f;
    for (int j = 0; j < ${XU}; j ++)
    {
        __global float* g_w_cpt_offset_col = g_w_cpt_offset + ${YU} * j;
        float sum = 0.0f;
        for (int i = 0; i < ${YU}; i ++)
            sum += lo_y_lambda[i] * (lo_y_pi[i] - g_w_kappa_offset[i] + g_w_cpt_offset_col[i]);
        w_lambda[j] = sum;
        outer_sum += sum;
    }
    // to calculate the final outcome then write back the outcome to global memory
    __global float* og_w_lambda_offset = og_w_lambda + (kWICol * ${Y} + kWIRow) * ${XU};
    for (int j = 0; j < ${XU}; j ++)
    {
        w_lambda[j] /= outer_sum;
        og_w_lambda_offset[j] = w_lambda[j];
    }

}