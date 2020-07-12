#include "util_ai.h"

/*距離 トーラス型*/
int monju::util_ai::distTorus(int win, int unit, int cycle)
{
	if (win > unit)
		std::swap(win, unit);
	return std::min(unit - win, (win + cycle) - unit);
}

/*距離 線形*/
int monju::util_ai::distLinear(int win, int unit)
{
	return std::abs(unit - win);
}

// 学習率を計算
float_t monju::util_ai::calcEta(int count, float_t eta)
{
	return eta / static_cast<float_t>(count);
}

/*近似関数 スムースステップ*/
float_t monju::util_ai::smoothstep(float_t edge0, float_t edge1, float_t x) {
	x = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
	return x * x * (3 - 2 * x);
}
