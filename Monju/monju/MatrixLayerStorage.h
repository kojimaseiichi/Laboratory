#pragma once
#ifndef _MONJU_MATRIX_LAYER_STORAGE_H__
#define _MONJU_MATRIX_LAYER_STORAGE_H__

#include <vector>
#include "Closable.h"
#include "Environment.h"
#include "ConcurrencyContext.h"
#include "GridMatrixStorage.h"
#include "ConcurrencyContext.h"
#include "Synchronizable.h"
#include "Extent.h"
#include "util_file.h"
#include "util_ai.h"

namespace monju {

	// 層間の勝者ユニットの分割表
	// CPTを計算・更新
	class MatrixLayerStorage : public GridMatrixStorage
	{
	private:
		std::unique_ptr<GridMatrixStorage> _ps;
		ConcurrencyContext _conc;
		GridExtent _gridExtent;
		GridExtent _gridExtentLambda;
		GridExtent _gridExtentKappa;
		Synchronizable _syn;

		const std::string _CONTINGENCY_TABLE = "ct"; // contingency table(int32_t)
		const std::string _CHANGE_TABLE= "cg"; // changes(int32_t)
		const std::string _CONDITIONAL_PROBABILITY_TABLE = "cpt"; // conditional probability table(float_t)
		const std::string _LAMBDA_VARIABLES = "lambda";
		const std::string _KAPPA_VARIABLES = "kappa";

	public:/*コンストラクタ*/
		MatrixLayerStorage(std::string fileName, LayerShape x, LayerShape y)
			: GridMatrixStorage(fileName),
			_conc(1)
		{
			_gridExtent.cross(x, y);
			_gridExtentLambda = _gridExtentKappa = _gridExtent;
			_gridExtentLambda.matrix.rows = 1;
			_gridExtentKappa.matrix.cols = 1;
			_prepareStorage();
		}
		~MatrixLayerStorage()
		{

		}

	public:/*公開メンバ*/
		void loadLambda(MatrixCm<float_t>& lambda)
		{
			readGrid<MatrixCm<float_t>>(_LAMBDA_VARIABLES, lambda);
		}

		void storeLambda(MatrixCm<float_t>& lambda)
		{
			writeGrid<MatrixCm<float_t>>(_LAMBDA_VARIABLES, lambda);
		}

		void loadKappa(MatrixCm<float_t>& kappa)
		{
			readGrid<MatrixCm<float_t>>(_KAPPA_VARIABLES, kappa);
		}

		void storeKappa(MatrixCm<float_t>& kappa)
		{
			writeGrid<MatrixCm<float_t>>(_KAPPA_VARIABLES, kappa);
		}

		void loadCpt(MatrixCm<float_t>& cpt)
		{
			readGrid<MatrixCm<float_t>>(_CONDITIONAL_PROBABILITY_TABLE, cpt);
		}

		void storeCpt(MatrixCm<float_t>& cpt)
		{
			writeGrid<MatrixCm<float_t>>(_CONDITIONAL_PROBABILITY_TABLE, cpt);
		}

		std::future<void> increment(std::weak_ptr<MatrixRm<int32_t>> winX, std::weak_ptr<MatrixRm<int32_t>> winY)
		{
			auto px = winX.lock();
			auto py = winY.lock();
			auto ax = _toVector(*px);
			auto ay = _toVector(*py);
			auto wrapper = [&] {
				this->coeffOp<int32_t>(
					_CHANGE_TABLE,
					ax,
					ay,
					[](const int32_t v)->int32_t {
						return v + 1;
					});
			};
			auto f = _conc.threadPool().submit(wrapper);
			return f;
		}
		std::future<void> merge()
		{
			auto future = _conc.threadPool().submit([&] {
				_merge();
				});
		}
		std::future<void> mergeAndCptUpdate()
		{
			auto topo = _cptTopologyTorus();
			_conc.threadPool().submit([&] {
				_mergeAndCptUpdate(topo);
				});
		}

		void calcCpt(std::weak_ptr<MatrixCm<float_t>> cpt)
		{
			// カウンティング情報をストレージから取得してCPTを計算
			auto p = cpt.lock();
			auto flat = _gridExtent.flattenCm();
			if (flat.rows != p->rows() || flat.cols != p->cols())
				throw MonjuException();
			MatrixCm<int32_t> count;
			count.resize(flat.rows, flat.cols);
			this->readGrid(_CONTINGENCY_TABLE, count);
			*p = count.cast<float_t>();
			const int matSize = _gridExtent.matrix.size();
			for (int gcol = 0; gcol < flat.cols; gcol++)
			{
				for (int grow = 0; grow < flat.rows; grow++)
				{
					auto v = p->block(grow * matSize, gcol, matSize, 1).reshaped(_gridExtent.matrix.rows, _gridExtent.matrix.cols);
					auto sum = v.sum();
					v.array() /= sum;
				}
			}
		}

#pragma region Helper
	private:/*ヘルパ*/
		void _prepareStorage()
		{
			this->prepare<int32_t>(_CONTINGENCY_TABLE, _gridExtent, kDensityRectangular, kColMajor, kColMajor);
			this->prepare<int32_t>(_CHANGE_TABLE, _gridExtent, kDensityRectangular, kColMajor, kColMajor);
			this->prepare<float_t>(_CONDITIONAL_PROBABILITY_TABLE, _gridExtent, kDensityRectangular, kColMajor, kColMajor);
			this->prepare<float_t>(_LAMBDA_VARIABLES, _gridExtentLambda, kDensityRectangular, kColMajor, kColMajor);
			this->prepare<float_t>(_KAPPA_VARIABLES, _gridExtentKappa, kDensityRectangular, kColMajor, kColMajor);
		}
		void _merge()
		{
			_grid_matrix_t entryCt, entryCg;
			if (!_findGridMatrix(_CONTINGENCY_TABLE, entryCt)) throw MonjuException();
			if (!_findGridMatrix(_CHANGE_TABLE, entryCg)) throw MonjuException();

			MatrixCm<int32_t> ct, cg;
			auto matShape = _gridExtent.matrix;
			ct.resize(matShape.rows, matShape.cols);
			cg.resizeLike(ct);
			for (int gcol = 0; gcol < _gridExtent.grid.cols; gcol++)
			{
				for (int grow = 0; grow < _gridExtent.grid.rows; grow++)
				{
					_cell_data_t cellCt, cellCg;
					_getCellData(entryCt, grow, gcol, cellCt);
					_getCellData(entryCg, grow, gcol, cellCg);
					_readMatrixData(entryCt, cellCt, ct);
					_readMatrixData(entryCg, cellCg, cg);
					ct += cg;
					_writeMatrixData(entryCt, cellCt, ct);
				}
			}
			_setCellDataZeros(entryCg);
		}
		std::function<int(int, int, int)> _cptTopologyTorus()
		{
			return [](int win, int col, int cycle) {
				if (win > col)
					std::swap(win, col);
				return std::min(col - win, (win + cycle) - col);
			};
		}
		void _mergeAndCptUpdate(const std::function<float_t(int, int, int)> cptTopology)
		{
			_grid_matrix_t entryCt, entryCg, entryCpt;
			if (!_findGridMatrix(_CONTINGENCY_TABLE, entryCt)) throw MonjuException();
			if (!_findGridMatrix(_CHANGE_TABLE, entryCg)) throw MonjuException();
			if (!_findGridMatrix(_CONDITIONAL_PROBABILITY_TABLE, entryCpt)) throw MonjuException();

			MatrixCm<int32_t> ct, cg;
			MatrixCm<float_t> cpt, d;
			auto matShape = _gridExtent.matrix;
			ct.resize(matShape.rows, matShape.cols);
			cg.resizeLike(ct);
			cpt.resize(matShape.rows, matShape.cols);
			d.resizeLike(cpt);
			// ループ(グリッド巡回)
			for (int gcol = 0; gcol < _gridExtent.grid.cols; gcol++)
			{
				for (int grow = 0; grow < _gridExtent.grid.rows; grow++)
				{
					_cell_data_t cellCt, cellCg, cellCpt;
					_getCellData(entryCt, grow, gcol, cellCt);
					_getCellData(entryCg, grow, gcol, cellCg);
					_getCellData(entryCpt, grow, gcol, cellCpt);

					_readMatrixData(entryCt, cellCt, ct);
					_readMatrixData(entryCg, cellCg, cg);
					_readMatrixData(entryCpt, cellCpt, cpt);

					ct += cg;
					_writeMatrixData(entryCt, cellCt, ct);

					d.setZero();
					// ループ(マトリックス巡回)
					// カウントされた要素をCPTに反映
					int cycle = static_cast<int>(matShape.cols);
					for (int col = 0; col < matShape.cols; col++)
					{
						for (int row = 0; row < matShape.rows; row++)
						{
							float_t count = static_cast<float_t>(cg(row, col));
							if (count == 0.f) continue;
							// 小さい値の係数をかける必要がある(勝ち数をかけて学習率を調整)
							float_t eta = 1.f / static_cast<float_t>(ct(row, col)) * count;
							util_ai::accumulateSomDeltaTorus(d, col, row, eta);
						}
					}
					cpt += d;
					util_ai::normalizeCptWithUpperPhiUnit(cpt);
					_writeMatrixData(entryCpt, cellCpt, cpt);
				}
			}
			_setCellDataZeros(entryCg);
		}
		std::vector<int32_t>&& _toVector(MatrixRm<int32_t>& win) const
		{
			std::vector<int32_t> v;
			for (int row = 0; row < win.rows(); row++)
				v.push_back(win(row, 0));
			return std::move(v);
		}
#pragma endregion Helper
	};
}

#endif // !_MONJU_MATRIX_LAYER_STORAGE_H__

