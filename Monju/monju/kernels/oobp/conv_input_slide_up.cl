/*
 # ATTENTION!
    You can write this source only with Latin-1 character set. DO NOT use Japanese characters.

 # SUMMARY


 # WORK ITEM
    [0] row-wise index of sliding window
    [1] col-wise index of sliding window

 # WORK GROUP
    N/A

 # TEMPLATE ARGUMENTS
    W:  width(x) of input image
    H:  height(y) of input image
    FW: width(x) of filter
    FH: height(Y) of filter
    SW: sliding steps width-wise direction
    SH: sliding steps height-wise direction
    Y:  number of nodes in the lower basis
    XU: number of units per node in the upper basis

 # PARAMETERS
    ig_image    image
    ig_cpt      cpt matrix
    og_x_lambda lambda

 # 
*/
__kernel void conv_input_slide_up_W${W}_H${H}_FW${FW}_FH${FH}_SW${SW}_SH${SH}_X${X}_XU${XU}(
    __global float* ig_image,
    __global float* ig_cpt,
    __global float* og_x_lambda
)
{
    // filter size: FW * FH
    // CPT size: FW * FH * XU
    const float kEpsilon = 0.0001;
    const size_t kSlideRow = get_global_id(0);
    const size_t kSlideCol = get_global_id(1);
    const size_t kImageRowStride = (${W} - ${FW} + ${SW}) / ${SW};
    const size_t kLinearId = kSlideRow * kImageRowStride + kSlideCol;
    const float* image_offset = ig_image + kSlideCol * ${SW} + kSlideRow * kImageRowStride;

    const int input_length = ${FW} * ${FH}; // filter size
    float input[${FW} * ${FH}];

    // load image fragment
    for (int h = 0; h < ${FH}; h ++)
    {
        for (int w = 0; w < ${FW}; w ++)
            input[h * ${FW} + w] = image_offset + w + h * kImageRowStride;
    }

    float lambda[${XU}];
    float b = 0.0f;
    __global float* g_cpt = ig_cpt;
    for (int x = 0; x < ${XU}; x ++)
    {
        float sum = 0.0f;
        for (int i = 0; i < ${FW} * ${FH}; i ++)
            sum += input[i] * g_cpt[i];
        lambda[x] = sum;
        b += sum;
        g_cpt += ${FW} * ${FH};
    }

    __global float* g_x_lambda = og_x_lambda + kLinearId * ${XU};
    for (int x = 0; x < ${XU}; x ++)
        g_x_lambda[x] = lambda[x] / b;
}