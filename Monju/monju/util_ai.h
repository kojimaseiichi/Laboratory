#pragma once
#ifndef _MONJU_UTIL_AI_H__
#define _MONJU_UTIL_AI_H__

#include <algorithm>

#include "MonjuTypes.h"
#include "Extent.h"

namespace monju
{
	namespace util_ai
	{

		/*���� �g�[���X�^*/
		int distTorus(int win, int unit, int cycle);

		/*���� ���`*/
		int distLinear(int win, int unit);

		// �w�K�����v�Z
		float_t calcEta(int count, float_t eta);

		/*�ߎ��֐� �X���[�X�X�e�b�v*/
		float_t smoothstep(float_t edge0, float_t edge1, float_t x);

		/*CPT���K�� */
		template <typename Matrix>
		void normalizeCptWithUpperPhiUnit(Matrix& cpt)
		{
			cpt.col(0).array() = 0.f;
			cpt.array() /= cpt.sum();
			cpt.col(0).array() = 1.f / static_cast<float_t>(cpt.rows());
		}

		/*CPT������ ���l���j�b�g�t��CPT�̃����_���ȏ�����*/
		template <typename Matrix>
		void randomCptWithUpperPhiUnit(Matrix& cpt)
		{
			cpt.setRandom();
			cpt = cpt.array().abs();
			normalizeCptWithUpperPhiUnit<Matrix>(cpt);
		}

		/*�����w�K */
		template <typename Matrix>
		void accumulateSomDeltaTorus(Matrix& cpt, int winX, int winY, float_t eta)
		{
			const int cols = static_cast<int>(cpt.cols());
			for (int col = 0; col < cols; col++)
			{
				int dist = distTorus(winX, col, cols); // ���҃��j�b�g�Ƃ̋���
				cpt(winY, col) = eta * smoothstep(1, 2, dist);
			}
		}
	}
}

#endif // !_MONJU_UTIL_AI_H__
