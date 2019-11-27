#define CPT_CELL_SIZE ${UU}*${XU}
#define CPT_GRID_SIZE ${U}*${X}

__kernel oobp3_down_step2_U${U}_X${X}_UU${UU}_XU${XU}(
    __global int* ig_jsize,
    __global int* ig_joff,
    __global int* ig_adj,
    __global int ig_jlen,
    __global float* ig_w_kappa,
    __global float* og_x_pi
)
{
    const int cell_row = get_global_id(0);

    __local int lo_jsize[${U}];
    __local int lo_joff[${U}];
    __local int lo_adj[CPT_GRID_SIZE];

    float w_kappa[${X}];

    // initialize final result
    float x_pi[${X}];
    for (int n = 0; n < ${X}; n ++)
        x_pi[n] = 0.0f;

    // loading from global to local
    if (cell_row == 0)
    {
        for (int col = 0; col < ${U}; col ++)
        {
            lo_jsize[col] = ig_jsize[col];
            lo_joff[col] = ig_joff[col];
        }
        for (int n = 0; n < ig_jlen; n ++)
            lo_adj[n] = ig_adj[n];
    }

    // waiting for all work-items to reach here
    barrier(CLK_LOCAL_MEM_FENCE);

    // calculation
    for (int col = 0; col < ${U}; col ++)
    {
        const int joff = lo_joff[col];
        __local float* adj_col_vec = lo_adj + joff; // col vector of the adjacent matrix
        const int jsize = lo_jsize[col];
        for (int n = 0; n < jsize; n ++)
        {
            const int adj = adj_col_vec[n];
            if (adj == cell_row)
            {
                __global float* g_w_kappa = ig_w_kappa + joff * ${XU};
                for (int k = 0; k < ${XU}; k ++)
                    x_pi[k] += g_w_kappa[k];
                break;
            }
            if (adj > cell_row)
                break;
        }
    }

    // storing pi
    __global float* g_x_pi = og_x_pi + cell_row * ${XU};
    for (int n = 0; n < ${XU}; n ++)
        g_x_pi[n] = x_pi[n];
}