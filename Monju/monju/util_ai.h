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

		/*距離 トーラス型*/
		int distTorus(int win, int unit, int cycle);

		/*距離 線形*/
		int distLinear(int win, int unit);

		// 学習率を計算
		float_t calcEta(int count, float_t eta);

		/*近似関数 スムースステップ*/
		float_t smoothstep(float_t edge0, float_t edge1, float_t x);

		/*CPT正規化 */
		template <typename Matrix>
		void normalizeCptWithUpperPhiUnit(Matrix& cpt)
		{
			cpt.col(0).array() = 0.f;
			cpt.array() /= cpt.sum();
			cpt.col(0).array() = 1.f / static_cast<float_t>(cpt.rows());
		}

		/*CPT初期化 Φ値ユニット付きCPTのランダムな初期化*/
		template <typename Matrix>
		void randomCptWithUpperPhiUnit(Matrix& cpt)
		{
			cpt.setRandom();
			cpt = cpt.array().abs();
			normalizeCptWithUpperPhiUnit<Matrix>(cpt);
		}

		/*競合学習 */
		template <typename Matrix>
		void accumulateSomDeltaTorus(Matrix& cpt, int winX, int winY, float_t eta)
		{
			const int cols = static_cast<int>(cpt.cols());
			for (int col = 0; col < cols; col++)
			{
				int dist = distTorus(winX, col, cols); // 勝者ユニットとの距離
				cpt(winY, col) = eta * smoothstep(1, 2, dist);
			}
		}
	}
}

#endif // !_MONJU_UTIL_AI_H__
