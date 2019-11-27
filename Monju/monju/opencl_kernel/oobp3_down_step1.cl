#define CPT_CELL_SIZE ${UU}*${XU}

__kernel oobp3_down_step1_U${U}_X${X}_UU${UU}_XU${XU}(
    __global int* ig_jsize,
    __global int* ig_joff,
    __global int* ig_jadj,
    __global int ig_jlen,
    __global float* ig_u_rho,
    __global float* ig_w_cpt,
    __global float* ig_w_lambda,
    __global float* og_w_kappa
)
{
    const int cell_col = get_global_id(1);

    const int jsize = ig_jsize[cell_col];
    const int joff = ig_joff[cell_col];
    __global int* g_jadj = ig_jadj + joff;

    float u_rho[${UU}];
    float w_cpt[CPT_CELL_SIZE];
    float w_lambda[${UU}];
    float w_pi[${UU}]; // tempo
    float w_kappa[${XU}];
    float x_pi[${XU}];

    // loading rho in upper-basis
    __global float* g_u_rho = ig_u_rho + ${UU} * cell_col;
    for (int n < 0; n < ${UU}; n ++)
        u_rho[n] = g_u_rho[n];

    // 
    for (int i = 0; i < jsize; i ++)
    {
        const int adj = g_jadj[i];
        // loading
        __global float* g_w_lambda = ig_w_lambda + ${UU} * (joff + i);
        for (int n = 0; n < ${UU}; n ++)
            w_lambda[n] = g_w_lambda[n];
        __global float* g_w_cpt = ig_w_cpt + CPT_CELL_SIZE * (joff + i);
        for (int n = 0; n < CPT_CELL_SIZE; n ++)
            w_cpt[n] = g_w_cpt[n];
        
        // calculation
        float sum = 0.0f;
        for (int col = 0; col < ${UU}; col ++)
        {
            float pi = u_rho[col] / w_lambda[col];
            w_pi[col] = pi;
            sum += pi;
        }
        for (int col = 0; col < ${UU}; col ++)
            w_pi[col] /= sum;
        
        for (int row = 0; row < ${XU}; row ++)
            w_kappa[row] = 0;
        for (int col = 0; col < ${UU}; col ++)
        {
            float pi = w_pi[col];
            float* cpt_col_vec = w_cpt + col * ${XU};
            for (int row = 0; row < ${XU}; row ++)
                w_kappa[row] += cpt_col_vec[row] * pi;
        }

        // storing kappa in the lower-basis
        __global float* g_w_kappa = og_w_kappa + ${XU} * (joff + i);
        for (int row = 0; row < ${XU}; row ++)
            g_w_kappa[row] = w_kappa[row];
    }
}