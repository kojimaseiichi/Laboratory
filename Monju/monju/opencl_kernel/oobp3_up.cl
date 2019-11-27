
__kernel void oobp3_up_X${X}_Y${Y}_XU${XU}_YU${YU}(
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
    // const int cell_row = get_global_id(0);
    const int cell_col = get_global_id(1);
    const int CPT_CELL_SIZE = ${XU} * ${YU};

    const int jent_size   = ig_jsize[cell_col];
    const int jent_offset = ig_joff[cell_col];
    __global int* g_jadj = ig_jadj + jent_offset;

    float y_lambda[${YU}];
    float y_pi[${YU}];
    float w_cpt[CPT_CELL_SIZE];
    float w_kappa[${YU}];
    float w_lambda[${XU}];
    float x_lambda[${XU}];
    for (int n = 0; n < ${XU}; n ++)
        x_lambda[n] = 1.0f;
    for (int i = 0; i < jent_size; i ++)
    {
        const int adj = g_jadj[i];
        // load lambda, pi and kappa from the lower basis
        __global float* g_y_lambda = ig_y_lambda + ${YU} * adj;
        __global float* g_y_pi = ig_y_pi + ${YU} * ajd;
        __global float* g_w_kappa = ig_w_kappa + ${YU} * (jent_offset + i);
        for (int n = 0; n < ${YU}; n ++)
        {
            y_lambda = g_y_lambda[n];
            y_pi = g_y_pi[n];
            w_kappa = g_w_kappa[n];
        }
        // load CPT cell
        __global float* g_w_cpt = ig_w_cpt + CPT_CELL_SIZE * (jent_offset + i);
        for (int n = 0; n < CPT_CELL_SIZE; n ++)
            w_cpt[n] = g_w_cpt[n];
        
        float sum_outer = 0.0f;
        for (int col = 0; col < ${XU}; col ++)
        {
            float* cpt_col_vec = w_cpt + col * ${YU};
            float sum = 0.0f;
            for (int row = 0; row < ${YU}; row ++)
                sum += y_lambda[row] * (y_pi[row] - w_kappa[row] + cpt_col_vec[row]);
            sum_outer += sum;
            w_lambda[col] = sum;
        }
        // final calc and storing lambda in inter-basis
        __global float* g_w_lambda = og_w_lambda + ${XU} * (jent_offset + i);
        for (int col = 0; col < ${XU}; col ++)
        {
            float la = w_lambda[col] / sum_outer;
            x_lambda[col] *= la;
            g_w_lambda[col] = la;
        }
    }
    // storing lambda in x-basis
    __global float* g_x_lambda = og_x_lambda + ${XU} * cell_col;
    for (int n = 0; n < ${XU}; n ++)
        g_x_lambda[n] = x_lambda[n];
}