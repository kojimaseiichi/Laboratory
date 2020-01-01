#include "util_math.h"
#include <cmath>


float monju::util_math::round_n(float number, int n)
{
	number = number * pow((double)10, (double)(n - 1)); //四捨五入したい値を10の(n-1)乗倍する。
	number = round(number); //小数点以下を四捨五入する。
	number /= pow((double)10, (double)(n - 1)); //10の(n-1)乗で割る。
	return number;
}
