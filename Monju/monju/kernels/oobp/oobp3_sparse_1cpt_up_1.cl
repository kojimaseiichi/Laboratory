/*
 # ATTENTION!
    You can write this source only with Latin-1 character set. DO NOT use Japanese characters.

 # SUMMARY
    This kernel is to compute the beysian stochastic propagation that is performed through the sparse-connected network by OOBP algorithm.
    You may need to call oobp3_full_up_2() to obtain LAMBDA from "og_w_lambda" in the basis after this kernel execution done

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
                    (3) rowwise order
                    (4) colwise order
    offset          offset of sparse array
    ig_y_rho        RHO variable in the lower basis
    ig_y_lambda     LAMBDA variable in the lower basis
    ig_w_one_cpt    the CPT matrix that intervenes between two bases
    ig_w_kappa      KAPPA variable that intervenes between two bases and is near the lower basis
    og_w_lambda     LAMBDA variable that intervenes between two bases and is near the upper basis

 */

typedef struct __attribute__((packed)) cell_addr
{
    int grid_row;
    int grid_col;
    int cell_index;
};

/*
definition in host code

#pragma pack(push, 1)
typedef struct _cell_addr
{
    cl_int grid_row;
    cl_int grid_col;
    cl_int cell_index;
} cell_addr;
#pragma pack(pop)
*/

__kernel void oobp3_sparse_1cpt_up_1_X${X}_Y${Y}_XU${XU}_YU${YU}(
    __global cell_addr* ig_cell_addr_list,
    size_t offset,
    __global float* ig_y_lambda,
    __global float* ig_y_pi,
    __global float* ig_w_one_cpt,
    __global float* ig_w_kappa,
    __global float* og_w_lambda
)
{
    const float kEpsilon = 0.0001;
    const size_t kCellIndex = get_global_linear_id() + offset;
    const size_t kCptSize = ${XU} * ${YU};

    // get cell info
    cell_addr cell = ig_cell_addr_list[kCellIndex];

    // no cell assigned
    if (cell.grid_row < 0 || cell.grid_col < 0)
        return;

    // to load variable that is shared among the work group
    __global float* g_y_pi      = ig_y_pi       + (cell.grid_row * ${YU});
    __global float* g_y_lambda  = ig_y_lambda   + (cell.grid_row * ${YU});
    __global float* g_w_kappa   = ig_w_kappa    + (cell.cell_index * ${YU});
    __global float* g_w_lambda  = og_w_lambda   + (cell.cell_index * ${XU});

    // to perform algorithm for every work items
    float w_lambda[${XU}];  // attention! this variable that is belonged to the upper basis
    float B = 0.0f;
    __global float* g_w_cpt_col_vec = ig_w_one_cpt;
    for (int j = 0; j < ${XU}; j ++)
    {
        float lambda = 0.0f;
        for (int i = 0; i < ${YU}; i ++)
            lambda += g_y_lambda[i] * (g_y_pi[i] - g_w_kappa[i] + g_w_cpt_col_vec[i]);
        w_lambda[j] = lambda;
        B += sum;
        g_w_cpt_col_vec += ${YU};
    }
    // to calculate the final outcome then write back the outcome to global memory
    B = max(B, kEpsilon);
    for (int j = 0; j < ${XU}; j ++)
        g_w_lambda[j] = w_lambda[j] / B;
}1