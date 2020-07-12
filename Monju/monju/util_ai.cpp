#include "util_ai.h"

/*���� �g�[���X�^*/
int monju::util_ai::distTorus(int win, int unit, int cycle)
{
	if (win > unit)
		std::swap(win, unit);
	return std::min(unit - win, (win + cycle) - unit);
}

/*���� ���`*/
int monju::util_ai::distLinear(int win, int unit)
{
	return std::abs(unit - win);
}

// �w�K�����v�Z
float_t monju::util_ai::calcEta(int count, float_t eta)
{
	return eta / static_cast<float_t>(count);
}

/*�ߎ��֐� �X���[�X�X�e�b�v*/
float_t monju::util_ai::smoothstep(float_t edge0, float_t edge1, float_t x) {
	x = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
	return x * x * (3 - 2 * x);
}
