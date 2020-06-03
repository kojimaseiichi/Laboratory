#pragma once
#ifndef _MONJU_FULL_CONNECTED_GRID_CPT_H__
#define _MONJU_FULL_CONNECTED_GRID_CPT_H__

#include "MonjuTypes.h"
#include "util_eigen.h"
#include "util_math.h"
#include "Extent.h"
#include "GridMatrixStorage.h"

namespace monju {

	/// いくつからの種類の学習アルゴリズムを実装
	/// 関数命名ルール
	/// SOMトポロジー_学習ルール
	class FullConnectedGridCpt
	{
	private:
		std::string _id;
		LayerShape
			_shapeX,
			_shapeY;
		std::shared_ptr<MatrixCm<float_t>>
			_cpt,			// グリッドCPT（４階テンソル）
			_deltaCpt;		// グリッドCPT増分
		const size_t _kCellSize;	// セルサイズ
		const float
			_coefficientNeighborGrid,	// ガウシアン関数の分散の２乗
			_coefficientNeighborCell;
		const std::string 
			_kCpt = "cpt",
			_kDeltaCpt = "delta-cpt";

	public:
		std::string id() const { return _id; }
		std::weak_ptr<MatrixCm<float_t>> cpt() { return _cpt; }
		std::weak_ptr<MatrixCm<float_t>> deltaCpt() { return _deltaCpt; }
		LayerShape shapeX() const { return _shapeX; }
		LayerShape shapeY() const { return _shapeY; }

	public:
		FullConnectedGridCpt(
			std::string id,
			LayerShape shapeX,
			LayerShape shapeY,
			float coefficientNeighborGrid,
			float coefficientNeighborCell
		) :
			_kCellSize(static_cast<size_t>(shapeX.units.size()) * shapeY.units.size()),
			_coefficientNeighborGrid(coefficientNeighborGrid),
			_coefficientNeighborCell(coefficientNeighborCell)
		{
			_id = id;
			_shapeX = shapeX;
			_shapeY = shapeY;
			const size_t rows = _kCellSize * _shapeY.nodes.size();

			_cpt = std::make_shared<MatrixCm<float_t>>();
			_deltaCpt = std::make_shared<MatrixCm<float_t>>();

			_cpt->resize(rows, shapeX.nodes.size());
			_cpt->setZero();

			_deltaCpt->resize(rows, shapeX.nodes.size());
			_deltaCpt->setZero();
		}
		void store(std::string dir)
		{
			std::string extension = "mat";
			{
				auto s = _prepareStorage(dir);
				s->writeGrid(_kCpt, *_cpt);
				s->writeGrid(_kDeltaCpt, *_deltaCpt);
			}
			util_eigen::write_binary(dir, _id, "cpt", extension, *_cpt);
			util_eigen::write_binary(dir, _id, "delta-cpt", extension, *_deltaCpt);
		}
		void load(std::string dir)
		{
			const size_t rows = _kCellSize * _shapeY.nodes.size();
			std::string extension = "mat";
			{
				auto s = _prepareStorage(dir);
				s->readGrid(_kCpt, *_cpt);
				s->readGrid(_kDeltaCpt, *_deltaCpt);
			}
		}
		void initRandom()
		{
			// CPT
			_cpt->setRandom();
			*_cpt = _cpt->array().abs();
			for (size_t col = 0; col < _shapeX.nodes.size(); col++)
			{
				for (size_t row = 0; row < _shapeY.nodes.size(); row++)
				{
					auto mm = _cpt
						->block(row * _kCellSize, col, _kCellSize, 1)
						.reshaped(_shapeY.units.size(), _shapeX.units.size());
					mm.array().rowwise() /= mm.colwise().sum().array();
				}
			}
			// delta CPT
			_deltaCpt->setZero();
		}
		void train(
			std::weak_ptr<MatrixRm<int32_t>> winX,
			std::weak_ptr<MatrixRm<int32_t>> winY,
			std::weak_ptr<MatrixCm<float_t>> wLambdaY,
			float_t learningRate
		)
		{
			// lambdaYで最大寄与のy=argmaxλyを計算
			// セル(y,*)に対してCPT更新（近傍学習１）
			// セル(y,*)の近傍セルに対してCPT更新（近傍学習２）

			auto pWinX = winX.lock();
			auto pWinY = winY.lock();
			auto pWLambdaY = wLambdaY.lock();

			for (size_t gridCol = 0; gridCol < _shapeX.nodes.size(); gridCol++)
			{
				// 勝者セル決定
				int winX = pWinX->coeff(0, gridCol);
				Eigen::Index winGridRow, winGridCol;
				pWLambdaY
					->block(0, gridCol, _shapeX.units.size() * _shapeY.nodes.size(), 1)
					.reshaped(_shapeY.nodes.size(), _shapeX.units.size())
					.col(pWinX->coeff(0, gridCol))
					.maxCoeff(&winGridRow, &winGridCol);
				// 勝者セルの学習
				_trainCell(winGridRow, gridCol, pWinY->coeff(0, winGridRow), winX, learningRate);
				// 近傍セルの学習
				for (int gridRowOffset = 1; gridRowOffset < _shapeY.nodes.size(); gridRowOffset++)
				{
					float neiboirGrid = util_math::approx2Gaussian(gridRowOffset, _coefficientNeighborGrid) * learningRate;	// 近傍
					if (neiboirGrid <= 0.001f)
						break;
					int rowA = winGridRow - gridRowOffset;
					int rowB = winGridRow + gridRowOffset;
					bool stop = true;
					if (rowA >= 0)
					{
						stop = false;
						_trainCell(rowA, gridCol, pWinY->coeff(0, rowA), winX, neiboirGrid);
					}
					if (rowB < _shapeY.units.size())
					{
						stop = false;
						_trainCell(rowB, gridCol, pWinY->coeff(0, rowB), winX, neiboirGrid);
					}
					if (stop)
						break;
				}
			}
		}

		void addDelta()
		{
			// CPTの増分を足す

			_cpt->array() += _deltaCpt->array();
			_deltaCpt->setZero();
		}

		bool containsNan()
		{
			bool a = util_eigen::contains_nan<MatrixCm<float_t>>(_cpt);
			bool b = util_eigen::contains_nan<MatrixCm<float_t>>(_deltaCpt);
			return a || b;
		}
	private:
		std::unique_ptr<GridMatrixStorage> _prepareStorage(const std::string& dir)
		{
			std::unique_ptr<GridMatrixStorage> s = std::make_unique<GridMatrixStorage>(util_file::combine(dir, _id, "mdb"));
			GridExtent e(Extent(_shapeY.nodes.size(), _shapeX.nodes.size()), Extent(_shapeY.units.size(), _shapeX.units.size()));
			s->prepare<float_t>(_kCpt, e, kDensityRectangular, kColMajor, kColMajor);
			s->prepare<float_t>(_kDeltaCpt, e, kDensityRectangular, kColMajor, kColMajor);
			std::move(s);
		}
		void _trainCell(int gridRow, int gridCol, int winY, int winX, float learningRate)
		{
			auto cell = _deltaCpt
				->block(gridRow * _kCellSize, gridCol, _kCellSize, 1)
				.reshaped(_shapeY.units.size(), _shapeX.units.size());
			cell.coeffRef(winY, winX) += learningRate;
			// 近傍学習
			for (int cellCol = 1; cellCol < _shapeX.units.size(); cellCol++)
			{
				float neighbor = util_math::approx2Gaussian(cellCol, _coefficientNeighborCell) * learningRate;
				if (neighbor <= 0.001f)
					break;
				int colA = winX - cellCol;
				int colB = winX + cellCol;
				bool stop = true;
				if (colA >= 0)
				{
					stop = false;
					cell.coeffRef(winY, colA) += neighbor;
				}
				if (colB < _shapeX.units.size())
				{
					stop = false;
					cell.coeffRef(winY, colB) += neighbor;
				}
				if (stop)
					break;
			}
		}


		// コピー禁止・ムーブ禁止
	public:
		FullConnectedGridCpt(const FullConnectedGridCpt&) = delete;
		FullConnectedGridCpt(FullConnectedGridCpt&&) = delete;
		FullConnectedGridCpt& operator =(const FullConnectedGridCpt&) = delete;
		FullConnectedGridCpt& operator =(FullConnectedGridCpt&&) = delete;
	};

}

#endif // !_MONJU_FULL_CONNECTED_GRID_CPT_H__
