#pragma once
#ifndef _MONJU_BAYESIAN_EDGE_H__
#define _MONJU_BAYESIAN_EDGE_H__

#include "Synchronizable.h"
#include "MonjuTypes.h"
#include "VariableKind.h"
#include "util_eigen.h"
#include "FullConnectedGridCpt.h"

namespace monju {

	// 基底間のデータをフィルニ保存
	// 保持するデータ
	// ・ベイズ計算的な変数（CPT/OOBP）
	class BayesianEdge : public Synchronizable
	{
	private:
		std::string _id;
		UniformBasisShape _shapeX, _shapeY;

		std::shared_ptr<MatrixCm<float_t>>
			_lambda,				// λ（ノード数Y * ユニット数Y, ノード数X）
			_kappa;					// κ（ノード数Y * ユニット数X, ノード数X）

		FullConnectedGridCpt _cpt;				// 重み行列

	public:
		std::string id() const { return _id; }
		UniformBasisShape shapeX() const { return _shapeX; }
		UniformBasisShape shapeY() const { return _shapeY; }
		std::weak_ptr<MatrixCm<float_t>> lambda() { return _lambda; }
		std::weak_ptr<MatrixCm<float_t>> kappa() { return _kappa; }
		FullConnectedGridCpt& cpt() { return _cpt; }

	public:
		BayesianEdge(
			std::string id,
			UniformBasisShape shapeX,
			UniformBasisShape shapeY,
			float coefficientNeighborGrid,
			float coefficientNeighborCell
		);
		~BayesianEdge();
		void initRandom();
		void store(std::string dir);
		void load(std::string dir);
		bool containsNan()
		{
			bool a = util_eigen::contains_nan<MatrixCm<float_t>>(_lambda);
			bool b = util_eigen::contains_nan<MatrixCm<float_t>>(_kappa);
			bool c = _cpt.containsNan();
			return a || b || c;
		}

	private:
		void _setRandomProb(std::shared_ptr<MatrixCm<float_t>> m, int unitsPerNode, int oppositNodes);

		// コピー禁止・ムーブ禁止
	public:
		BayesianEdge(const BayesianEdge&) = delete;
		BayesianEdge(BayesianEdge&&) = delete;
		BayesianEdge& operator =(const BayesianEdge&) = delete;
		BayesianEdge& operator =(BayesianEdge&&) = delete;
	};
}

#endif // !_MONJU_BAYESIAN_EDGE_H__
