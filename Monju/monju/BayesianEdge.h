#pragma once
#ifndef _MONJU_BAYESIAN_EDGE_H__
#define _MONJU_BAYESIAN_EDGE_H__
#include "Synchronizable.h"
#include "MonjuTypes.h"
#include "VariableKind.h"
#include "util_eigen.h"


namespace monju {

	// 基底間のデータをフィルニ保存
	// 保持するデータ
	// ・ベイズ計算的な変数（CPT/OOBP）
	class BayesianEdge : public Synchronizable
	{
	private:
		std::string _id;
		int
			_kNodesX,				// 基底のノード数(X)
			_kUnitsPerNodeX,		// ノード当たりのユニット数(X)
			_kNodesY,				// 基底のノード数(Y)
			_kUnitsPerNodeY;		// ノード当たりのユニット数(Y)

		MatrixCm<float_t>
			_lambda,				// λ（ノード数Y * ユニット数Y, ノード数X）
			_kappa;					// κ（ノード数Y * ユニット数X, ノード数X）

	public:
		std::string id() const { return _id; }
		int nodesX() const { return _kNodesX; }
		int unitsPerNodeX() const { return _kUnitsPerNodeX; }
		int nodesY() const { return _kNodesY; }
		int unitsPerNodeY() const { return _kUnitsPerNodeY; }
		MatrixCm<float_t> lambda() const { return _lambda; }
		MatrixCm<float_t> kappa() const { return _kappa; }


	public:
		BayesianEdge(const BayesianEdge&) = delete;
		BayesianEdge& operator =(const BayesianEdge&) = delete;

		BayesianEdge(
			std::string id,
			int nodes_x,
			int units_per_node_x,
			int nodes_y,
			int units_per_node_y
		);
		~BayesianEdge();
		void initRandom();
		void store(std::string dir);
		void load(std::string dir);

	private:
		void _setRandomProb(MatrixCm<float_t>& m, int unitsPerNode, int oppositNodes);
	};
}

#endif // !_MONJU_BAYESIAN_EDGE_H__
