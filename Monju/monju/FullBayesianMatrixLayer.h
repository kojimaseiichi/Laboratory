#pragma once
#ifndef _MONJU_FULL_BAYESIAN_MATRIX_LAYER_H__
#define _MONJU_FULL_BAYESIAN_MATRIX_LAYER_H__

#include "MonjuTypes.h"
#include "Synchronizable.h"
#include "VariableKind.h"
#include "util_eigen.h"
#include "FullConnectedGridCpt.h"
#include "GridMatrixStorage.h"
#include "Environment.h"
#include "BelLayer.h"

namespace monju {

	// 基底間のデータをフィルニ保存
	// 保持するデータ
	// ・ベイズ計算的な変数（CPT/OOBP）
	class FullBayesianMatrixLayer : public Synchronizable
	{
	protected:
		std::shared_ptr<Environment> _env;
		std::string _id;
		LayerShape _shapeX, _shapeY;
		GridExtent _gridExtent;
		std::shared_ptr<MatrixCm<float_t>>
			_lambda,				// λ（ノード数Y * ユニット数Y, ノード数X）
			_kappa,					// κ（ノード数Y * ユニット数X, ノード数X）
			_cpt;					// 重み行列（ユニット数Y * ユニット数X * ノード数Y, ノード数X）

	public: // プロパティ
		std::string id() const { return _id; }
		LayerShape shapeX() const { return _shapeX; }
		LayerShape shapeY() const { return _shapeY; }
		std::weak_ptr<MatrixCm<float_t>> lambda() { return _lambda; }
		std::weak_ptr<MatrixCm<float_t>> kappa() { return _kappa; }
		std::weak_ptr<MatrixCm<float_t>> cpt() { return _cpt; }

	public:
		FullBayesianMatrixLayer(
			std::weak_ptr<Environment> env,
			std::string id,
			LayerShape shapeX,
			LayerShape shapeY
		);
		~FullBayesianMatrixLayer();
		void initVariables();
		void store();
		void load();
		bool containsNan() const;
		void copyData(const FullBayesianMatrixLayer& o);
		void performUp(BelLayer& x, BelLayer& y)
		{
			GridExtent gextCpt, gextLambda, gextKappa;
			_makeGridExtent(gextCpt, gextLambda, gextKappa);

			auto pi = y.pi().lock();
			auto la = y.lambda().lock();
			for (int gcol = 0; gcol < _gridExtent.grid.cols; gcol++)
			{
				for (int grow = 0; grow < _gridExtent.grid.rows; grow++)
				{
					auto lo_pi = pi->block(grow, 0, 1, _gridExtent.matrix.rows).reshaped(_gridExtent.matrix.rows, 1);
					auto lo_la = la->block(grow, 0, 1, _gridExtent.matrix.rows).reshaped(_gridExtent.matrix.rows, 1);
					auto lo_kappa = _kappa->block(grow * gextKappa.matrix.size(), gcol, gextKappa.matrix.size(), 1).reshaped(gextKappa.matrix.size(), 1);
					auto lo_lambda = _lambda->block(grow * gextLambda.matrix.size(), gcol, gextLambda.matrix.size(), 1).reshaped(1, gextLambda.matrix.size());

					auto lo_cpt = _cpt->block(grow * gextCpt.matrix.size(), gcol, gextCpt.matrix.size(), 1).reshaped(gextCpt.matrix.rows, gextCpt.matrix.cols);
					auto b = (lo_cpt.colwise() + (lo_pi - lo_kappa).col(0)).array().colwise() * lo_la.col(0).array();
					auto c = b.colwise().sum();
					lo_lambda = c.array() / c.sum();
				}
			}
			auto dla = x.lambda().lock();
			auto r = x.r().lock();
			for (int gcol = 0; gcol < _gridExtent.grid.cols; gcol++)
			{
				auto lambda = _lambda->block(0, gcol, gextLambda.matrix.size() * gextLambda.grid.rows, 1).reshaped(gextLambda.matrix.size(), gextLambda.grid.rows).transpose();
				auto a = lambda(0, 0);
				auto b = lambda(1, 0);
				auto c = lambda(2, 0);
				auto d = lambda(3, 0);

				auto u = lambda.array().colwise().prod() * r->row(gcol).array();
				auto e = u(0, 0);
				auto f = u(0, 1);
				dla->row(gcol) = u;
			}
		}
		void performDown(BelLayer& x, BelLayer& y)
		{
			GridExtent gextCpt, gextLambda, gextKappa;
			_makeGridExtent(gextCpt, gextLambda, gextKappa);

			auto rho = x.rho().lock();
			for (int gcol = 0; gcol < _gridExtent.grid.cols; gcol++)
			{
				auto lo_rho = rho->row(gcol);
				for (int grow = 0; grow < _gridExtent.grid.rows; grow++)
				{
					auto lo_lambda = _lambda->block(grow * _gridExtent.matrix.cols, gcol, _gridExtent.matrix.cols, 1).reshaped(1, _gridExtent.matrix.cols).row(0);
					auto lo_cpt = _cpt->block(grow * gextCpt.matrix.size(), gcol, gextCpt.matrix.size(), 1).reshaped(gextCpt.matrix.rows, gextCpt.matrix.cols);
					auto lo_kappa = _kappa->block(grow * _gridExtent.matrix.rows, gcol, _gridExtent.matrix.rows, 1);

					auto a = lo_rho.array() / lo_lambda.array();
					auto pi = a.array() / a.sum();
					auto b = lo_cpt.array().rowwise() * pi;
					lo_kappa = b.rowwise().sum();
				}
			}
			auto pi = y.pi().lock();
			decltype(pi)::element_type cp;
			cp.resizeLike(*pi);
			cp.setZero();
			for (int grow = 0; grow < _gridExtent.grid.rows; grow++)
			{
				auto kappa = _kappa->block(grow * gextKappa.matrix.size(), 0, gextKappa.matrix.size(), gextKappa.grid.cols).reshaped(gextKappa.matrix.size(), gextKappa.grid.cols);
				cp.row(grow) = kappa.rowwise().sum();
			}
			*pi = cp;
		}

	protected:
		std::string _dataFileName() const;
		void _makeGridExtent(GridExtent& cpt, GridExtent& lambda, GridExtent& kappa);
		void _initHostMemory();
		void _prepareStorage(GridMatrixStorage& storage, GridExtent cptExtent, GridExtent lambdaExtent, GridExtent kappaExtent);
		void _setRandom();
		void _storeMatrices(std::string& dir);
		void _loadMatrices(std::string& dir);

		// コピー禁止・ムーブ禁止
	public:
		FullBayesianMatrixLayer(const FullBayesianMatrixLayer&) = delete;
		FullBayesianMatrixLayer(FullBayesianMatrixLayer&&) = delete;
		FullBayesianMatrixLayer& operator =(const FullBayesianMatrixLayer&) = delete;
		FullBayesianMatrixLayer& operator =(FullBayesianMatrixLayer&&) = delete;
	};
}

#endif // !_MONJU_FULL_BAYESIAN_MATRIX_LAYER_H__
