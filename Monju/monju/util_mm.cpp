#include "util_mm.h"
#include <codecvt>

__int32 monju::util_mm::__calc_size(__m128i _shape)
{
	__m128i __one = _mm_set1_epi32(1);
	__m128i __a = _mm_max_epi32(_shape, __one);
	__int32 v = 1;
	for (int i = 0; i < 4; i++)
		v *= __a.m128i_i32[i];
	return v;
}

bool monju::util_mm::__check_not_negative(__m128i v)
{
	__m128i __zero = _mm_setzero_si128();
	__m128i __cmpgt = _mm_cmpgt_epi32(__zero, v); // 0 > v
	__m128i __ones = _mm_set1_epi32(0xffffff);
	int test = _mm_testz_si128(__cmpgt, __ones);
	return test == 1;
}

bool monju::util_mm::__check_equals(__m128i a, __m128i b)
{
	__m128i __eq = _mm_cmpeq_epi32(a, b);
	__m128i __one = _mm_set1_epi32(1);
	int test = _mm_testc_si128(__eq, __one);
	return test == 1;
}

