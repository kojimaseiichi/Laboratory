#pragma once
#ifndef _MONJU_FULL_BAYESIAN_MATRIX_LAYER_H__
#define _MONJU_FULL_BAYESIAN_MATRIX_LAYER_H__

#include "MonjuTypes.h"
#include "Synchronizable.h"
#include "VariableKind.h"
#include "util_eigen.h"
#include "FullConnectedGridCpt.h"

namespace monju {

	// 基底間のデータをフィルニ保存
	// 保持するデータ
	// ・ベイズ計算的な変数（CPT/OOBP）
	class FullBayesianMatrixLayer : public Synchronizable
	{
	private:
		std::string _id;
		UniformBasisShape _shapeX, _shapeY;

		std::shared_ptr<MatrixCm<float_t>>
			_lambda,				// λ（ノード数Y * ユニット数Y, ノード数X）
			_kappa,					// κ（ノード数Y * ユニット数X, ノード数X）
			_cpt;					// 重み行列（ユニット数Y * ユニット数X * ノード数Y, ノード数X）

		int _kCellSize;

	public: // プロパティ
		std::string id() const { return _id; }
		UniformBasisShape shapeX() const { return _shapeX; }
		UniformBasisShape shapeY() const { return _shapeY; }
		std::weak_ptr<MatrixCm<float_t>> lambda() { return _lambda; }
		std::weak_ptr<MatrixCm<float_t>> kappa() { return _kappa; }
		std::weak_ptr<MatrixCm<float_t>> cpt() { return _cpt; }

	public:
		FullBayesianMatrixLayer(
			std::string id,
			UniformBasisShape shapeX,
			UniformBasisShape shapeY
		)
		{
			_id = id;
			_shapeX = shapeX;
			_shapeY = shapeY;
			_kCellSize = shapeX.units * shapeY.units;

			_allocEigenVariables();
			_setRandom();
		}
		~FullBayesianMatrixLayer()
		{

		}
		void store(std::string dir)
		{
			std::string extension = "mat2";
			util_eigen::write_binary(dir, _id, "lambda", extension, *_lambda);
			util_eigen::write_binary(dir, _id, "kappa", extension, *_kappa);
			util_eigen::write_binary(dir, _id, "cpt", extension, *_cpt);
		}
		void load(std::string dir)
		{
			std::string extension = "mat2";
			if (util_eigen::read_binary(dir, _id, "lambda", extension, *_lambda) == false)
				util_eigen::init_matrix_zero(*_lambda, _shapeY.nodes * _shapeX.units, _shapeX.nodes);
			if (util_eigen::read_binary(dir, _id, "kappa", extension, *_kappa) == false)
				util_eigen::init_matrix_zero(*_kappa, _shapeY.nodes * _shapeY.units, _shapeX.nodes);
			if (util_eigen::read_binary(dir, _id, "cpt", extension, *_cpt) == false)
				util_eigen::init_matrix_zero(*_cpt, static_cast<Eigen::Index>(_kCellSize) * _shapeY.units, _shapeX.nodes);
		}
		bool containsNan()
		{
			bool a = util_eigen::contains_nan<MatrixCm<float_t>>(_lambda);
			bool b = util_eigen::contains_nan<MatrixCm<float_t>>(_kappa);
			bool c = util_eigen::contains_nan<MatrixCm<float_t>>(_cpt);
			return a || b || c;
		}

	private:
		void _allocEigenVariables()
		{
			_lambda = std::make_shared<MatrixCm<float_t>>();
			_kappa = std::make_shared<MatrixCm<float_t>>();
			_cpt = std::make_shared<MatrixCm<float_t>>();

			_lambda->resize(static_cast<Eigen::Index>(_shapeY.nodes) * _shapeX.units, _shapeX.nodes);
			_kappa->resize(static_cast<Eigen::Index>(_shapeY.nodes) * _shapeY.units, _shapeX.nodes);
			_cpt->resize(static_cast<Eigen::Index>(_kCellSize) * _shapeY.nodes, _shapeX.nodes);
		}
		void _setRandom()
		{
			// ランダムに初期化、確率分布として正規化
			util_eigen::set_stratum_prob_ramdom(_lambda.get(), _shapeX.units);
			util_eigen::set_stratum_prob_ramdom(_kappa.get(), _shapeY.units);
			util_eigen::set_cpt_random(_cpt.get(), _kCellSize, _shapeY.units, _shapeX.units);
		}

		// コピー禁止・ムーブ禁止
	public:
		FullBayesianMatrixLayer(const FullBayesianMatrixLayer&) = delete;
		FullBayesianMatrixLayer(FullBayesianMatrixLayer&&) = delete;
		FullBayesianMatrixLayer& operator =(const FullBayesianMatrixLayer&) = delete;
		FullBayesianMatrixLayer& operator =(FullBayesianMatrixLayer&&) = delete;
	};
}

#endif // !_MONJU_FULL_BAYESIAN_MATRIX_LAYER_H__
