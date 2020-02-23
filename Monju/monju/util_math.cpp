#include "util_math.h"
#include <cmath>


float monju::util_math::round_n(float number, int n)
{
	number = number * pow((double)10, (double)(n - 1)); //四捨五入したい値を10の(n-1)乗倍する。
	number = round(number); //小数点以下を四捨五入する。
	number /= pow((double)10, (double)(n - 1)); //10の(n-1)乗で割る。
	return number;
}

float monju::util_math::approx2Exp(float x)
{
	// マクローリン展開（２階）
	return 1 + x + x * x / 4;
}

float monju::util_math::approx2Gaussian(float x, float variance)
{
	// ガウシアン関数（２階近似）
	return approx2Exp((x * x) / (2 * variance * variance));
}
