#include "util_math.h"
#include <cmath>


float monju::util_math::round_n(float number, int n)
{
	number = number * pow((double)10, (double)(n - 1)); //�l�̌ܓ��������l��10��(n-1)��{����B
	number = round(number); //�����_�ȉ����l�̌ܓ�����B
	number /= pow((double)10, (double)(n - 1)); //10��(n-1)��Ŋ���B
	return number;
}

float monju::util_math::approx2Exp(float x)
{
	// �}�N���[�����W�J�i�Q�K�j
	return 1 + x + x * x / 4;
}

float monju::util_math::approx2Gaussian(float x, float variance)
{
	// �K�E�V�A���֐��i�Q�K�ߎ��j
	return approx2Exp((x * x) / (2 * variance * variance));
}
