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
		UniformBasisShape _shape;
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
		UniformBasisShape shape() const { return _shape; }
		std::weak_ptr<MatrixRm<float_t>> lambda() const { return _lambda; }
		std::weak_ptr<MatrixRm<float_t>> pi() const { return _pi; };
		std::weak_ptr<MatrixRm<float_t>> rho() const { return _rho; }
		std::weak_ptr<MatrixRm<float_t>> r() const { return _r; }
		std::weak_ptr<MatrixRm<float_t>> bel() const { return _bel; }
		std::weak_ptr<MatrixRm<int32_t>> win() const { return _win; }

	public:
		BayesianNode(std::string id, UniformBasisShape shape)
		{
			_id = id;
			_shape = shape;

			_lambda = std::make_shared<MatrixRm<float_t>>();
			_pi = std::make_shared<MatrixRm<float_t>>();
			_rho = std::make_shared<MatrixRm<float_t>>();
			_r = std::make_shared<MatrixRm<float_t>>();
			_bel = std::make_shared<MatrixRm<float_t>>();
			_win = std::make_shared<MatrixRm<int32_t>>();

			_lambda->resize(shape.nodes, shape.units);
			_pi->resize(shape.nodes, shape.units);
			_rho->resize(shape.nodes, shape.units);
			_r->resize(shape.nodes, shape.units);
			_bel->resize(shape.nodes, shape.units);
			_win->resize(shape.nodes, 1);
		}
		~BayesianNode()
		{
		}

		void initRandom();
		void store(std::string dir);
		void load(std::string dir);
		void findWinnerOfBEL()
		{
			for (Eigen::Index nRow = 0; nRow < _bel->rows(); nRow++)
			{
				auto row = _bel->row(nRow);
				MatrixRm<float_t>::Index maxarg;
				row.maxCoeff(&maxarg);
				(*_win)(nRow, 0) = static_cast<int32_t>(maxarg);
			}
		}
		bool containsNan()
		{
			bool a = util_eigen::contains_nan<MatrixRm<float_t>>(_lambda);
			bool b = util_eigen::contains_nan<MatrixRm<float_t>>(_pi);
			bool c = util_eigen::contains_nan<MatrixRm<float_t>>(_rho);
			bool d = util_eigen::contains_nan<MatrixRm<float_t>>(_r);
			bool e = util_eigen::contains_nan<MatrixRm<float_t>>(_bel);
			bool f = util_eigen::contains_nan<MatrixRm<int32_t>>(_win);
			return a || b || c || d || e || f;
		}
	private:
		void _setRandomProb(std::shared_ptr<MatrixRm<float_t>> m);

		// コピー禁止・ムーブ禁止
	public:
		BayesianNode(const BayesianNode&) = delete;
		BayesianNode(BayesianNode&&) = delete;
		BayesianNode& operator =(const BayesianNode&) = delete;
		BayesianNode& operator =(BayesianNode&&) = delete;
	};

}

#endif // !_MONJU_BAYESIAN_NODE_H__
