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

		UniformBasisShape
			_shapeX,
			_shapeY;
		std::shared_ptr<MatrixCm<byte_t>> _incidence;			// 接続行列
		std::shared_ptr<MatrixCm<cl_short>> _edgeArray;			// カーネルの計算で必要	
		std::shared_ptr<MatrixCm<float_t>> _cptArray;			// グリッドCPT（１個の列ベクトルが１個のCPT）
		std::shared_ptr<SparseMatrixCm<float_t>> _deltaCpt;		// グリッドCPT増分
		const size_t _kCellSize;	// セルサイズ
		const float
			_coefficientNeighborGrid,	// ガウシアン関数の分散の２乗
			_coefficientNeighborCell;

		RectangularGridMatrixStorage<float_t> _storage;

	public:
		SparseGridCpt(
			std::string id,
			Environment& environment,
			UniformBasisShape shapeX,
			UniformBasisShape shapeY,
			float coefficientNeighborGrid,
			float coefficientNeighborCell
		) :
			_kCellSize(static_cast<size_t>(shapeX.units)* shapeY.units),
			_coefficientNeighborGrid(coefficientNeighborGrid),
			_coefficientNeighborCell(coefficientNeighborCell)
		{
			_id = id;
			_pEnv = &environment;
			_shapeX = shapeX;
			_shapeY = shapeY;
			const size_t rows = _kCellSize * _shapeY.nodes;

			_incidence = std::make_shared<MatrixCm<std::uint8_t>>();
			_edgeArray = std::make_shared<MatrixCm<cl_short>>();
			_cptArray = std::make_shared<MatrixCm<float_t>>();
			_deltaCpt = std::make_shared<SparseMatrixCm<float_t>>();
			
			_incidence->resize(_shapeY.nodes, _shapeX.nodes);
			_incidence->setZero();	// 初期状態で接続なし
			_deltaCpt->resize(_shapeY.nodes * _kCellSize, _shapeX.nodes);
			_deltaCpt->setZero();

			auto fileName = util_file::combine(_pEnv->info().workFolder, _id + "cpt", "grid");
			_storage.create(fileName, shapeY.nodes, shapeX.nodes, shapeY.units, shapeX.units);
		}
		~SparseGridCpt()
		{
			_storage.close();
		}

	private:
		void _storeCpt(bool storing)
		{
			for (Eigen::Index col = 0; col < _edgeArray->cols(); col++)
			{
				const auto kGridRow = (*_edgeArray)(0, col);
				const auto kGridCol = (*_edgeArray)(1, col);
				auto m = _cptArray->col(col).reshaped(_shapeY.units, _shapeX.units);
				// MatrixColMajorAccessor<float_t, std::remove_reference<decltype(m)>::type> a(m);
				if (storing)
					_storage.writeCellCm(m, kGridRow, kGridCol);
				else
					_storage.readCellCm(m, kGridRow, kGridCol);
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
		void _setIncidence(size_t filterRows, size_t filterCols, size_t strideRow, size_t strideCol)
		{
			_incidence->setZero();
			for (size_t row = 0; row < _shapeX.extent.rows; row++)
			{
				for (size_t col = 0; col < _shapeX.extent.cols; col++)
				{
					auto minorMatrix = _incidence->col(row + col * _shapeX.extent.rows)
						.reshaped(_shapeY.extent.rows, _shapeY.extent.cols);
					for (int fx = 0; fx < filterCols; fx++)
					{
						for (int fy = 0; fy < filterCols; fy++)
						{
							minorMatrix.coeffRef(row * strideRow + fx, col * strideCol + fy) = static_cast<byte_t>(1);
						}
					}
				}
			}
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
					auto m = _cptArray->col(count).reshaped(_shapeY.units, _shapeX.units);
					// MatrixColMajorAccessor<float_t, std::remove_reference<decltype(m)>::type> a(m);
					_storage.readCellCm(m, row, col);
					count++;
				}
			}
		}
	};
}

#endif // !_MONJU_SPARSE_GRID_CPT_H__
