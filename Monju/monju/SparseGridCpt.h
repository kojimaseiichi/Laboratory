#pragma once
#ifndef _MONJU_SPARSE_GRID_CPT_H__
#define _MONJU_SPARSE_GRID_CPT_H__

#include <vector>
#include <CL/cl.h>
#include "MonjuTypes.h"
#include "util_eigen.h"
#include "util_math.h"
#include "GridMatrixStorage.h"
#include "Environment.h"

namespace monju
{
	class SparseGridCpt
	{
	private:
		using byte_t = std::uint8_t;

		std::string _id;
		Environment* _pEnv;

		LayerStruct
			_shapeX,
			_shapeY;
		std::shared_ptr<MatrixCm<byte_t>> _incidence;			// 接続行列 ノード接続
		std::shared_ptr<MatrixCm<cl_short>> _edgeArray;			// カーネルの計算で必要	
		std::shared_ptr<MatrixCm<float_t>> _cptArray;			// グリッドCPT（１個の列ベクトルが１個のCPT）
		std::shared_ptr<SparseMatrixCm<float_t>> _deltaCpt;		// グリッドCPT増分
		const size_t _kCellSize;	// セルサイズ
		const float
			_coefficientNeighborGrid,	// ガウシアン関数の分散の２乗
			_coefficientNeighborCell;

		std::unique_ptr<GridMatrixStorage> _storage;

	public:
		SparseGridCpt(
			std::string id,
			Environment& environment,
			LayerStruct shapeX,
			LayerStruct shapeY,
			float coefficientNeighborGrid,
			float coefficientNeighborCell
		) :
			_kCellSize(static_cast<size_t>(shapeX.units.size())* shapeY.units.size()),
			_coefficientNeighborGrid(coefficientNeighborGrid),
			_coefficientNeighborCell(coefficientNeighborCell)
		{
			_id = id;
			_pEnv = &environment;
			_shapeX = shapeX;
			_shapeY = shapeY;
			const size_t rows = _kCellSize * _shapeY.nodes.size();

			_incidence = std::make_shared<MatrixCm<std::uint8_t>>();
			_edgeArray = std::make_shared<MatrixCm<cl_short>>();
			_cptArray = std::make_shared<MatrixCm<float_t>>();
			_deltaCpt = std::make_shared<SparseMatrixCm<float_t>>();
			
			// ストレージ確保
			auto fileName = util_file::combine(_pEnv->info().workFolder, _id, "dbm");
			_storage = std::make_unique<GridMatrixStorage>(fileName);

			// CPT
			GridExtent cptExtent;
			cptExtent.setCpt(_shapeX, _shapeY);
			_storage->prepare<float_t>("cpt", cptExtent, kDensityRectangular, kColMajor, kColMajor);
			auto cptMatShape = cptExtent.flattenCm();
			_deltaCpt->resize(cptMatShape.rows, cptMatShape.cols);
			_deltaCpt->setZero();

			// Incidence
			Extent incidenceExtent(_shapeY.nodes.size(), _shapeX.nodes.size());
			_storage->prepare<uint8_t>("incidence", incidenceExtent, kRowMajor);
			_incidence->resize(incidenceExtent.rows, incidenceExtent.cols);
			_incidence->setZero();	// 初期状態で接続なし
		}
		~SparseGridCpt()
		{
		}

	private:
		void _storeCpt(bool storing)
		{
			for (Eigen::Index col = 0; col < _edgeArray->cols(); col++)
			{
				const auto kGridRow = (*_edgeArray)(0, col);
				const auto kGridCol = (*_edgeArray)(1, col);
				auto m = _cptArray->col(col).reshaped(_shapeY.units.size(), _shapeX.units.size());
				if (storing)
					_storage->writeMatrix("cpt", kGridRow, kGridCol, m);
				else
					_storage->readMatrix("cpt", kGridRow, kGridCol, m);
			}
		}
		void _store()
		{
			util_eigen::write_binary(_pEnv->info().workFolder, _id, "incidence", "mat", *_incidence);
			util_eigen::write_binary(_pEnv->info().workFolder, _id, "edges", "mat", *_edgeArray);
			_storeCpt(true);
		}
		void _load()
		{
			util_eigen::write_binary(_pEnv->info().workFolder, _id, "incidence", "mat", *_incidence);
			util_eigen::write_binary(_pEnv->info().workFolder, _id, "edges", "mat", *_edgeArray);
			_storeCpt(false);
			_deltaCpt->setZero();
		}

		void _buildCptArray()
		{
			size_t len = (_incidence->array() == (byte_t)1).count();	// 接続エッジ数
			_edgeArray->resize(2, len);
			_cptArray->resize(_kCellSize, len);
			_deltaCpt->setZero();
			int count = 0;
			for (int row = 0; row < _incidence->rows(); row++)
			{
				for (int col = 0; col < _incidence->cols(); col++)
				{
					auto m = _cptArray->col(count).reshaped(_shapeY.units.size(), _shapeX.units.size());
					_storage->readMatrix("cpt", row, col, m);
					count++;
				}
			}
		}
	};
}

#endif // !_MONJU_SPARSE_GRID_CPT_H__
