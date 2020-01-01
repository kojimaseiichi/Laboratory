#include "util_math.h"
#include <cmath>


float monju::util_math::round_n(float number, int n)
{
	number = number * pow((double)10, (double)(n - 1)); //�l�̌ܓ��������l��10��(n-1)��{����B
	number = round(number); //�����_�ȉ����l�̌ܓ�����B
	number /= pow((double)10, (double)(n - 1)); //10��(n-1)��Ŋ���B
	return number;
}
