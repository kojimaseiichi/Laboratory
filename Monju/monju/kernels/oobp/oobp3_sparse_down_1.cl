/*
 # ATTENTION!
    We can write this source only with Latin-1 character set. DO NOT use Japanese characters.

 # SUMMARY
    This kernel is to compute the bayesian stochastic propagation that is performed through the sparse-connected network by OOBP algorithm.
    You may need to call oobp3_sparse_down_2() to obtain LAMBDA from "og_w_spa_kappa" in the basis after this kernel execution done

 # WORK ITEM
    [0] threads per every cells
    
 # WORK GROUP
    

 # TEMPLATE ARGUMENTS
    X:  number of nodes in the upper basis
    Y:  number of nodes in the lower basis
    XU: number of units per node in the upper basis
    YU: number of units per node in the lower basis

 # PARAMETERS
    ig_cell_inf     array of tupple that consists of
                    (1) grid row
                    (2) grid column
                    (3) order in the lower basis
                    (4) order in the upper basis
    offset          offset of sparse array
    ig_x_rho        RHO variable in the upper basis
    ig_w_spa_cpt    the CPT matrix that intervenes between two bases
    ig_w_lambda     LAMBDA variable that intervenes between two bases and is near the upper basis
    og_w_kappa      KAPPA variable that intervenes between two bases and is near the lower basis

*/
__kernel void oobp3_sparse_down_1_X${X}_Y${Y}_XU${XU}_YU${YU}(
    __global int* ig_cell_inf,
    size_t offset,
    __global float* ig_x_rho,
    __global float* ig_w_spa_cpt,
    __global float* ig_w_lambda,
    __global float* og_w_kappa
)
{
    const float kEpsilon = 0.0001f;
    const size_t kCellIndex = get_global_linear_id() + offset;
    const size_t kCptSize = ${XU} * ${YU};

    // get cell info
    __global int* g_cell_inf = ig_cell_inf + kCellIndex * 4 ;
    const int kGridRow = g_cell_inf[0];
    const int kGridCol = g_cell_inf[1];
    const int kOrderLower = g_cell_inf[2];
    const int kOrderUpper = g_cell_inf[3];

    // no cell assigned
    if (kGridRow < 0 || kGridCol < 0)
        return;

    // to load variable that is shared among the work group
    __global float* g_x_rho = ig_x_rho + (kGridCol * ${XU});
    __global float* g_w_cpt = ig_w_spa_cpt + kCellIndex * kCptSize;
    __global float* g_w_lambda = ig_w_lambda + kOrderLower * ${XU};
    __global float* g_w_kappa = og_w_kappa + kOrderUpper * ${YU};

    // to calculate the standardized sumation of "rho(x) / lambda_y(x)"
    float w_pi[${XU}];
    float sum = 0.0f;
    for (int j = 0; j < ${XU}; j ++)
    {
        float a = g_x_rho[j] / g_w_lambda[j];
        sum += a;
        w_pi[j] = a;
    }
    sum = max(sum, kEpsilon);
    for (int j = 0; j < ${XU}; j ++)
        w_pi[j] /= sum;

    // to calculate the sumation of "cpt_xy(y,x) * pi_y(x)"
    float w_kappa[${YU}];
    for (int i = 0; i < ${YU}; i ++)
        w_kappa[i] = 0.0f;
    __global float* g_w_cpt_col_vec = g_w_cpt;
    for (int j = 0; j < ${XU}; j ++)
    {
        float pi = w_pi[j];
        for (int i = 0; i < ${YU}; i ++)
            w_kappa[i] += pi * g_w_cpt_col_vec[i];
        g_w_cpt_col_vec += ${YU};
    }
    
    // to store the outcome
    for (int i = 0; i < ${YU}; i ++)
        g_w_kappa[i] = w_kappa[i];
}
