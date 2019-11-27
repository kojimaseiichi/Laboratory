// a = a + b * c
__kernel void fmad_${WS}(
	__global float* g_a,
	__global float* g_b,
	__global float* g_c
)
{
	// thread info
	const int global_id = get_global_id(0);

	// local memory
	float lo_a[${WS}];
	float lo_b[${WS}];
	float lo_c[${WS}];

	int offset = global_id * ${WS};
	for (int n = 0; n < ${WS}; n ++)
	{
		lo_a[n] = g_a[offset + n];
		lo_b[n] = g_b[offset + n];
		lo_c[n] = g_c[offset + n];
	}
}
