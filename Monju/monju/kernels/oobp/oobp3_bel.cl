/*
 # ATTENTION!
    You can write this source only with Latin-1 character set. DO NOT use Japanese characters.

 # SUMMARY
    This kernel is to get the final state of the basis by calculating the BEL 

 # WORK ITEM
    [0] threads for every nodes in the basis
    [1] 1

 # WORK GROUP
    N/A

 # TEMPLATE ARGUMENTS
    X:  number of nodes in the basis
    XU: number of units per node in the basis

 # PARAMETERS
    ig_x_lambda     LAMBDA variable in the basis
    ig_x_pi         PI variable in the basis
    og_x_rho        RHO variable in the basis
    og_x_BEL        BEL variable in the basis
    og_x_win        index of the winner unit

*/
__kernel void oobp3_bel_X${X}_XU${XU}(
    __global float* ig_x_lambda,
    __global float* ig_x_pi,
    __global float* og_x_rho,
    __global float* og_x_BEL,
    __global int* og_x_win
)
{
    const float kEpsilon = 0.0001f;
    const int kNode = get_global_id(0);

    const int kOffset = kNode * ${XU};
    __global float* g_x_lambda_current = ig_x_lambda + kOffset;
    __global float* g_x_pi_current = ig_x_pi + kOffset;
    __global float* g_x_rho = og_x_rho + kOffset;
    __global float* g_x_bel = og_x_BEL + kOffset;

    float rho_arr[${XU}];
    float sum = 0.0f;
    int win = -1;
    float max_rho = 0;

    for (int i = 0; i < ${XU}; i ++)
    {
        float a = g_x_lambda_current[i] * g_x_pi_current[i];
        rho_arr[i] = a;
        sum += a;
    }
    for (int i = 0; i < ${XU}; i ++)
    {
        float rho = rho_arr[i];
        g_x_rho[i] = rho;
        g_x_bel[i] = rho / max(sum, kEpsilon);
        // if (rho > max_rho)
        // {
        //     max_rho = rho;
        //     win = i;
        // }
        int compare = isgreater(rho, max_rho); //rho > max_rho
        win = select(win, i, compare);
        max_rho = select(max_rho, rho, compare);
    }
    og_x_win[kNode] = win;
}
