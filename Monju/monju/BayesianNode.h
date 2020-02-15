#pragma once
#ifndef _MONJU_BAYESIAN_NODE_H__
#define _MONJU_BAYESIAN_NODE_H__
#include "Synchronizable.h"
#include "MonjuTypes.h"
#include "VariableKind.h"
#include "util_eigen.h"
#include "DeviceMemory.h"

namespace monju {

	// 基底のデータをファイルに保存
	// 保持するデータ
	// ・ベイズ計算的の変数（OOBP）
	// ・（予定）予測符号化（PredNet的な）
	class BayesianNode : public Synchronizable
	{
	private:

		std::string _id;
		int 
			_nodes,				// 基底のノード数
			_units_per_node;	// ノード当たりのユニット数
		std::shared_ptr<MatrixRm<float_t>>
			_lambda,	// λ_X(x)
			_pi,		// π_X(x)
			_rho,		// ρ_X(x)
			_r,			// ペナルティc項_X(x)
			_bel;		// BEL_X(x)
		std::shared_ptr<MatrixRm<int32_t>>
			_win;		// 勝者ユニット

	public:
		std::string id() const { return _id; }
		int nodes() const { return _nodes; }
		int unitsPerNode() const { return _units_per_node; }
		std::weak_ptr<MatrixRm<float_t>> lambda() const { return _lambda; }
		std::weak_ptr<MatrixRm<float_t>> pi() const { return _pi; };
		std::weak_ptr<MatrixRm<float_t>> rho() const { return _rho; }
		std::weak_ptr<MatrixRm<float_t>> r() const { return _r; }
		std::weak_ptr<MatrixRm<float_t>> bel() const { return _bel; }
		std::weak_ptr<MatrixRm<int32_t>> win() const { return _win; }

	public:
		BayesianNode(const BayesianNode&) = delete;
		BayesianNode& operator =(const BayesianNode&) = delete;

		BayesianNode(std::string id, int nodes, int units_per_node)
		{
			_id = id;
			_nodes = nodes;
			_units_per_node = units_per_node;

			_lambda = std::make_shared<MatrixRm<float_t>>();
			_pi = std::make_shared<MatrixRm<float_t>>();
			_rho = std::make_shared<MatrixRm<float_t>>();
			_r = std::make_shared<MatrixRm<float_t>>();
			_bel = std::make_shared<MatrixRm<float_t>>();
			_win = std::make_shared<MatrixRm<int32_t>>();

			_lambda->resize(nodes, units_per_node);
			_pi->resize(nodes, units_per_node);
			_rho->resize(nodes, units_per_node);
			_r->resize(nodes, units_per_node);
			_bel->resize(nodes, units_per_node);
			_win->resize(nodes, 1);
		}
		~BayesianNode()
		{
		}

		void initRandom();
		void store(std::string dir);
		void load(std::string dir);
		void findWinnerOfBEL()
		{
			for (Eigen::Index nRow = 0; nRow < _bel.rows(); nRow++)
			{
				auto row = _bel.row(nRow);
				MatrixRm<float_t>::Index maxarg;
				row.maxCoeff(&maxarg);
				_win(nRow, 0) = static_cast<int32_t>(maxarg);
			}
		}
	private:
		void _setRandomProb(MatrixRm<float_t>& m);
	};

}

#endif // !_MONJU_BAYESIAN_NODE_H__
