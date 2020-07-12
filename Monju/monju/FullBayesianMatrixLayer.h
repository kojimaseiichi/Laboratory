#pragma once
#ifndef _MONJU_FULL_BAYESIAN_MATRIX_LAYER_H__
#define _MONJU_FULL_BAYESIAN_MATRIX_LAYER_H__

#include "MonjuTypes.h"
#include "Synchronizable.h"
#include "VariableKind.h"
#include "util_eigen.h"
#include "FullConnectedGridCpt.h"
#include "MatrixLayerStorage.h"
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
		std::unique_ptr<MatrixLayerStorage> _storage;


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
		void store(std::string path);
		void load();
		void load(std::string path);
		bool containsNan() const;
		void copyData(const FullBayesianMatrixLayer& o);
		void performUp(BelLayer& x, BelLayer& y);
		void performDown(BelLayer& x, BelLayer& y);

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
