#pragma once
#ifndef _MONJU_FULL_BAYESIAN_MATRIX_LAYER_H__
#define _MONJU_FULL_BAYESIAN_MATRIX_LAYER_H__

#include "MonjuTypes.h"
#include "Synchronizable.h"
#include "VariableKind.h"
#include "util_eigen.h"
#include "FullConnectedGridCpt.h"
#include "GridMatrixStorage.h"

namespace monju {

	// 基底間のデータをフィルニ保存
	// 保持するデータ
	// ・ベイズ計算的な変数（CPT/OOBP）
	class FullBayesianMatrixLayer : public Synchronizable
	{
	protected:
		std::string _id;
		LayerStruct _shapeX, _shapeY;

		std::shared_ptr<MatrixCm<float_t>>
			_lambda,				// λ（ノード数Y * ユニット数Y, ノード数X）
			_kappa,					// κ（ノード数Y * ユニット数X, ノード数X）
			_cpt;					// 重み行列（ユニット数Y * ユニット数X * ノード数Y, ノード数X）

		int _kCellSize;

	public: // プロパティ
		std::string id() const { return _id; }
		LayerStruct shapeX() const { return _shapeX; }
		LayerStruct shapeY() const { return _shapeY; }
		std::weak_ptr<MatrixCm<float_t>> lambda() { return _lambda; }
		std::weak_ptr<MatrixCm<float_t>> kappa() { return _kappa; }
		std::weak_ptr<MatrixCm<float_t>> cpt() { return _cpt; }

	public:
		FullBayesianMatrixLayer(
			std::string id,
			LayerStruct shapeX,
			LayerStruct shapeY
		)
		{
			_id = id;
			_shapeX = shapeX;
			_shapeY = shapeY;

			_calcCellSize();

			_allocEigenVariables();
			_setRandom();
		}
		FullBayesianMatrixLayer(std::string id, std::string dir)
		{
			_id = id;
			_loadInfo(dir);
			_calcCellSize();
			_loadEigen(dir);
		}
		~FullBayesianMatrixLayer()
		{

		}
		void save(std::string dir)
		{
			_storeInfo(dir);
			_storeEigen(dir);
		}
		bool containsNan()
		{
			bool a = util_eigen::contains_nan<MatrixCm<float_t>>(_lambda);
			bool b = util_eigen::contains_nan<MatrixCm<float_t>>(_kappa);
			bool c = util_eigen::contains_nan<MatrixCm<float_t>>(_cpt);
			return a || b || c;
		}

	protected:
		void _allocEigenVariables()
		{
			_lambda = std::make_shared<MatrixCm<float_t>>();
			_kappa = std::make_shared<MatrixCm<float_t>>();
			_cpt = std::make_shared<MatrixCm<float_t>>();

			_lambda->resize(static_cast<Eigen::Index>(_shapeY.nodes.size()) * _shapeX.units.size(), _shapeX.nodes.size());
			_kappa->resize(static_cast<Eigen::Index>(_shapeY.nodes.size()) * _shapeY.units.size(), _shapeX.nodes.size());
			_cpt->resize(static_cast<Eigen::Index>(_kCellSize) * _shapeY.nodes.size(), _shapeX.nodes.size());
		}
		void _setRandom()
		{
			// ランダムに初期化、確率分布として正規化
			util_eigen::set_stratum_prob_ramdom(_lambda.get(), _shapeX.units.size());
			util_eigen::set_stratum_prob_ramdom(_kappa.get(), _shapeY.units.size());
			util_eigen::set_cpt_random(_cpt.get(), _kCellSize, _shapeY.units.size(), _shapeX.units.size());
		}
		void _storeEigen(std::string& dir)
		{
			GridMatrixStorage data(util_file::combine(dir, _id, "dbm"));
			for (Eigen::Index col = 0; col < _shapeX.nodes.size(); col++)
			{
				for (Eigen::Index row = 0; row < _shapeY.nodes.size(); row++)
				{
					data.writeMatrix("lambda", row, col, _lambda->block(row * _shapeX.units.size(), col, _shapeX.units.size(), 1));
					data.writeMatrix("kappa", row, col, _kappa->block(row * _shapeY.units.size(), col, _shapeY.units.size(), 1));
					data.writeMatrix("cpt", row, col, _cpt->block(row * _kCellSize, col, _kCellSize, 1));
				}
			}
		}
		void _storeInfo(std::string& dir)
		{
			auto path = util_file::combine(dir, _id, "xml");
			std::ofstream file(path);
			boost::archive::text_oarchive ar(file);
			ar << boost::serialization::make_nvp("shapeX", _shapeX);
			ar << boost::serialization::make_nvp("shapeY", _shapeY);
		}
		void _loadEigen(std::string& dir)
		{
			GridMatrixStorage data(util_file::combine(dir, _id, "dbm"));
			for (Eigen::Index col = 0; col < _shapeX.nodes.size(); col++)
			{
				for (Eigen::Index row = 0; row < _shapeY.nodes.size(); row++)
				{
					data.readMatrix("lambda", row, col, _lambda->block(row * _shapeX.units.size(), col, _shapeX.units.size(), 1));
					data.readMatrix("kappa", row, col, _kappa->block(row * _shapeY.units.size(), col, _shapeY.units.size(), 1));
					data.readMatrix("cpt", row, col, _cpt->block(row * _kCellSize, col, _kCellSize, 1));
				}
			}
		}
		void _loadInfo(std::string& dir)
		{
			auto path = util_file::combine(dir, _id, "xml");
			std::ifstream file(path);
			boost::archive::text_iarchive ar(file);
			ar >> boost::serialization::make_nvp("shapeX", _shapeX);
			ar >> boost::serialization::make_nvp("shapeY", _shapeY);
		}
		void _calcCellSize()
		{
			_kCellSize = _shapeX.units.size() * _shapeY.units.size();
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
