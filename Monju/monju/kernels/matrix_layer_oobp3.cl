/********************************************************************
PHA : phase
CWP : col-wise pruning
*********************************************************************/

#define BRAIN_EPSILON 0x1.0p-5h

#define N_UNITS_UP ${N_UNITS_UP}
#define N_UNITS_DOWN ${N_UNITS_DOWN}


__kernel
void oobp_up_cwp_pha1(
    __global half* ig_y_lambda,
    __global half* ig_y_pi,
    __global half* ig_w_cpt_cwp,
    __global half* ig_w_kappa_cwp,
    __global short* ig_w_adj_list,
    constant short adj_list_len,
    __global half* og_w_lambda_cwp
)
{
    const size_t k_grid_row = get_global_id(0) + get_global_offset(0);
    const size_t k_grid_col = get_global_id(1) + get_global_offset(1);
    const size_t k_unit = get_global_id(2);

    __local half lambda[N_UNITS_DOWN];
    __local half pi[N_UNITS_DOWN];
    __local half kappa[N_UNITS_DOWN];
    if (k_unit == 0)
    {
        
    }
}

__kernel
void oobp_up_cwp_pha2(
    __global half* ig_w_lambda_cwp,
    constant short adj_list_len,
    __global half* ig_x_r,
    __global half* og_x_lambda
)
{

}