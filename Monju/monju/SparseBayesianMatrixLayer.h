#ifndef _MONJU_SPARSE_BAYESIAN_MATRIX_LAYER_H__
#define _MONJU_SPARSE_BAYESIAN_MATRIX_LAYER_H__

#include "MonjuTypes.h"
#include "Synchronizable.h"
#include "VariableKind.h"
#include "util_eigen.h"
#include "FullConnectedGridCpt.h"

namespace monju {

	// 基底間のデータをフィルニ保存
	// 保持するデータ
	// ・ベイズ計算的な変数（CPT/OOBP）
	class SparseBayesianMatrixLayer : public Synchronizable
	{
	protected:
		std::string _id;
		std::string _dir;	// ワークフォルダ
		UniformBasisShape _shapeX, _shapeY;

		std::shared_ptr<MatrixCm<float_t>>
			_lambda,				// λ（ノード数Y * ユニット数Y, ノード数X）
			_kappa,					// κ（ノード数Y * ユニット数X, ノード数X）
			_cpt;					// 重み行列（ユニット数Y * ユニット数X * ノード数Y, ノード数X）
		std::shared_ptr<MatrixCm<int8_t>>
			_adj;	// 隣接行列

		std::vector<cell_addr> _cellAddr;	// _adjから作成 _spLambda、_spKappa, _spCptと同期
		std::shared_ptr<MatrixCm<float_t>>
			_spLambda,				// λ（スパース・一時データ）
			_spKappa,				// κ（スパース・一時データ）
			_spCpt;					// CPT（スパース・一時データ）

		int _kCellSize;

	public: // プロパティ
		std::string id() const { return _id; }
		UniformBasisShape shapeX() const { return _shapeX; }
		UniformBasisShape shapeY() const { return _shapeY; }
		std::weak_ptr<MatrixCm<float_t>> lambda() { return _lambda; }
		std::weak_ptr<MatrixCm<float_t>> kappa() { return _kappa; }
		std::weak_ptr<MatrixCm<float_t>> cpt() { return _cpt; }
		std::weak_ptr<MatrixCm<int8_t>> adj() { return _adj; }
		std::weak_ptr<MatrixCm<float_t>> sparseLambda() { return _spLambda; }
		std::weak_ptr<MatrixCm<float_t>> sparseKappa() { return _spKappa; }
		std::weak_ptr<MatrixCm<float_t>> sparseCpt() { return _spCpt; }
		const std::vector<cell_addr> cellAddrList() { return _cellAddr; }

	public:
		SparseBayesianMatrixLayer(
			std::string id,
			std::string dir,
			UniformBasisShape shapeX,
			UniformBasisShape shapeY
		)
		{
			_id = id;
			_dir = dir;
			_shapeX = shapeX;
			_shapeY = shapeY;

			_calcCellSize();

			_allocSubstantialVariables();
			_allocOtherVariables();
			_setRandom();
		}
		SparseBayesianMatrixLayer(std::string id, std::string dir)
		{
			_id = id;
			_loadInfo();
			_calcCellSize();
			_loadSubstantialVariables();
		}
		~SparseBayesianMatrixLayer()
		{

		}
		void save()
		{
			// 全データ保存
			_storeInfo();
			_storeSubstantialVariables();
			_storeOtherVariables();
		}
		void load()
		{
			// 全データ復元
			_allocSubstantialVariables();
			_loadSubstantialVariables();
		}
		void unload()
		{
			_freeSubstantialVariables();
		}
		void sparse()
		{
			// λ, κ, CPTをメモリに確保してスパースデータを書き戻す
			// スパースデータを再構築
			// 
		}
		bool containsNan()
		{
			bool a = util_eigen::contains_nan<MatrixCm<float_t>>(_lambda);
			bool b = util_eigen::contains_nan<MatrixCm<float_t>>(_kappa);
			bool c = util_eigen::contains_nan<MatrixCm<float_t>>(_cpt);
			return a || b || c;
		}

	protected:
		void _allocSubstantialVariables()
		{
			// 実体のある変数
			_lambda = std::make_shared<MatrixCm<float_t>>();
			_kappa = std::make_shared<MatrixCm<float_t>>();
			_cpt = std::make_shared<MatrixCm<float_t>>();

			_lambda->resize(static_cast<Eigen::Index>(_shapeY.nodes) * _shapeX.units, _shapeX.nodes);
			_kappa->resize(static_cast<Eigen::Index>(_shapeY.nodes) * _shapeY.units, _shapeX.nodes);
			_cpt->resize(static_cast<Eigen::Index>(_kCellSize) * _shapeY.nodes, _shapeX.nodes);
		}
		void _freeSubstantialVariables()
		{
			_lambda.reset();
			_kappa.reset();
			_cpt.reset();
		}
		void _allocOtherVariables()
		{
			_adj = std::make_shared<MatrixCm<int8_t>>();
			_adj->resize(_shapeY.nodes, _shapeX.nodes);
			_adj->setZero();
		}
		void _setRandom()
		{
			// ランダムに初期化、確率分布として正規化
			util_eigen::set_stratum_prob_ramdom(_lambda.get(), _shapeX.units);
			util_eigen::set_stratum_prob_ramdom(_kappa.get(), _shapeY.units);
			util_eigen::set_cpt_random(_cpt.get(), _kCellSize, _shapeY.units, _shapeX.units);
		}
		void _storeSubstantialVariables()
		{
			if (_lambda)	util_eigen::write_binary(_dir, _id, "lambda", EIGEN_BINARY_FILE_EXT, *_lambda);
			if (_kappa)		util_eigen::write_binary(_dir, _id, "kappa", EIGEN_BINARY_FILE_EXT, *_kappa);
			if (_cpt)		util_eigen::write_binary(_dir, _id, "cpt", EIGEN_BINARY_FILE_EXT, *_cpt);
		}
		void _storeOtherVariables()
		{
			if (_adj)		util_eigen::write_binary(_dir, _id, "adj", EIGEN_BINARY_FILE_EXT, *_adj);
		}
		void _storeInfo()
		{
			auto path = util_file::combine(_dir, _id, "xml");
			std::ofstream file(path);
			boost::archive::text_oarchive ar(file);
			ar << boost::serialization::make_nvp("shapeX", _shapeX);
			ar << boost::serialization::make_nvp("shapeY", _shapeY);
		}
		void _loadSubstantialVariables()
		{
			if (util_eigen::read_binary(_dir, _id, "lambda", EIGEN_BINARY_FILE_EXT, *_lambda) == false)
				util_eigen::init_matrix_zero(*_lambda, _shapeY.nodes * _shapeX.units, _shapeX.nodes);
			if (util_eigen::read_binary(_dir, _id, "kappa", EIGEN_BINARY_FILE_EXT, *_kappa) == false)
				util_eigen::init_matrix_zero(*_kappa, _shapeY.nodes * _shapeY.units, _shapeX.nodes);
			if (util_eigen::read_binary(_dir, _id, "cpt", EIGEN_BINARY_FILE_EXT, *_cpt) == false)
				util_eigen::init_matrix_zero(*_cpt, static_cast<Eigen::Index>(_kCellSize) * _shapeY.units, _shapeX.nodes);
		}
		void _loadOtherVariables()
		{
			if (util_eigen::read_binary(_dir, _id, "adj", EIGEN_BINARY_FILE_EXT, *_adj) == false)
				util_eigen::init_matrix_zero(*_adj, static_cast<Eigen::Index>(_shapeY.nodes) * _shapeX.nodes);
		}
		void _loadInfo()
		{
			auto path = util_file::combine(_dir, _id, "xml");
			std::ifstream file(path);
			boost::archive::text_iarchive ar(file);
			ar >> boost::serialization::make_nvp("shapeX", _shapeX);
			ar >> boost::serialization::make_nvp("shapeY", _shapeY);
		}
		void _calcCellSize()
		{
			_kCellSize = _shapeX.units * _shapeY.units;
		}
		void _makeCellAddrList()
		{
			std::vector<cell_addr> list;
			cl_int count = 0;
			for (Eigen::Index row = 0; row < _adj->rows(); row++)
			{
				for (Eigen::Index col = 0; col < _adj->cols(); col++)
				{
					auto a = _adj->coeff(row, col);
					if (a == 0) continue;
					cell_addr ca;
					ca.grid_row = static_cast<cl_int>(row);
					ca.grid_col = static_cast<cl_int>(col);
					ca.cell_index = count;
					list.push_back(ca);
					count++;
				}
			}
			_cellAddr = list;
		}
		void _buildSparseVariables()
		{
			// 有効なエッジの
			_spLambda->resize(_shapeX.units, _cellAddr.size());
			_spLambda->setZero();
			_spKappa->resize(_shapeY.units, _cellAddr.size());
			_spKappa->setZero();
			_spCpt->resize(_kCellSize, _cellAddr.size());

			for (auto& a : _cellAddr)
				_spCpt->block(0, a.cell_index, _kCellSize, 1) = _cpt->block(static_cast<Eigen::Index>(_kCellSize) * a.grid_row, a.grid_col, _kCellSize, 1);
		}


		// コピー禁止・ムーブ禁止
	public:
		SparseBayesianMatrixLayer(const SparseBayesianMatrixLayer&) = delete;
		SparseBayesianMatrixLayer(SparseBayesianMatrixLayer&&) = delete;
		SparseBayesianMatrixLayer& operator =(const SparseBayesianMatrixLayer&) = delete;
		SparseBayesianMatrixLayer& operator =(SparseBayesianMatrixLayer&&) = delete;
	};
}

#endif // _MONJU_SPARSE_BAYESIAN_MATRIX_LAYER_H__