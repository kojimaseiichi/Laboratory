#pragma once
#ifndef _MONJU_GRID_CPT_H__
#define _MONJU_GRID_CPT_H__

#include "MonjuTypes.h"
#include "util_eigen.h"

namespace monju {

	/// いくつからの種類の学習アルゴリズムを実装
	/// 関数命名ルール
	/// SOMトポロジー_学習ルール
	class GridCpt
	{
	private:
		std::string _id;
		UniformBasisShape
			_shapeX,
			_shapeY;
		std::shared_ptr<MatrixCm<float_t>>
			_cpt,			// グリッドCPT（４階テンソル）
			_deltaCpt;		// グリッドCPT増分
		const int _kCellSize;	// セルサイズ

	public:
		std::string id() const { return _id; }
		std::weak_ptr<MatrixCm<float_t>> cpt() { return _cpt; }
		std::weak_ptr<MatrixCm<float_t>> deltaCpt() { return _deltaCpt; }

	public:
		GridCpt(
			std::string id,
			UniformBasisShape shapeX,
			UniformBasisShape shapeY
		):
			_kCellSize(shapeX.units * shapeY.units)
		{
			_id = id;
			_shapeX = shapeX;
			_shapeY = shapeY;
			const int rows = _kCellSize * _shapeY.nodes;
			
			_cpt = std::make_shared<MatrixCm<float_t>>();
			_deltaCpt = std::make_shared<MatrixCm<float_t>>();

			_cpt->resize(rows, shapeX.nodes);
			_cpt->setZero();

			_deltaCpt->resize(rows, shapeX.nodes);
			_deltaCpt->setZero();
		}
		void store(std::string dir)
		{
			std::string extension = "mat";
			util_eigen::write_binary(dir, _id, "cpt", extension, *_cpt);
			util_eigen::write_binary(dir, _id, "delta-cpt", extension, *_deltaCpt);
		}
		void load(std::string dir)
		{
			const int rows = _kCellSize * _shapeY.nodes;
			std::string extension = "mat";
			if (util_eigen::read_binary(dir, _id, "cpt", extension, *_cpt) == false)
				util_eigen::init_matrix_zero(*_cpt, rows, _shapeX.nodes);
			if (util_eigen::read_binary(dir, _id, "delta-cpt", extension, *_deltaCpt) == false)
				util_eigen::init_matrix_zero(*_deltaCpt, rows, _shapeX.nodes);
		}
		void initRandom()
		{
			// CPT
			_cpt->setRandom();
			*_cpt = _cpt->array().abs();
			for (int col = 0; col < _shapeX.nodes; col++)
			{
				for (int row = 0; row < _shapeY.nodes; row++)
				{
					auto map = Eigen::Map<MatrixCm<float_t>>(_cpt->col(col).data() + row * static_cast<uintptr_t>(_kCellSize), _shapeY.nodes, _shapeX.units);
					map.array().rowwise() /= map.colwise().sum().array();
				}
			}
			// delta CPT
			_deltaCpt->setZero();
		}
		void winnerTakerAll(
			MatrixRm<int32_t>& winX,
			MatrixRm<int32_t>& winY,
			float learningRate
		)
		{
			/// 単純なWTA
			/// 上位基底と下位基底の勝者ユニットに対して指定した学習率で増分
			/// 上下の勝者ユニットをu,vとしたときに次のように更新
			/// w(v,u) = (1-a)w(v,u) + a
			
			for (int col = 0; col < _shapeX.nodes; col++)
			{
				int winUnitX = winX(col, 0);
				for (int row = 0; row < _shapeY.nodes; row++)
				{
					int winUnitY = winY(row, 0);
					auto map = Eigen::Map<MatrixCm<float_t>>(_cpt->col(col).data() + static_cast<int>(row * _kCellSize), _shapeY.units, _shapeX.units);
					auto mapDelta = Eigen::Map<MatrixCm<float_t>>(_deltaCpt->col(col).data() + static_cast<int>(row * _kCellSize), _shapeY.units, _shapeX.units);
					mapDelta(winUnitY, winUnitX) += learningRate * (1 - map(winUnitY, winUnitX));
				}
			}
		}

		void winnerTakerAll(
			MatrixRm<int32_t>& winX,
			MatrixRm<int32_t>& winY,
			MatrixCm<float_t>& lambdaX,
			float learningRate
		)
		{
			// 勝者セルのみ学習するWTA

			for (int col = 0; col < _shapeX.nodes; col++)
			{
				int winUnitX = winX(col, 0);
				auto mapLambda = lambdaX.col(col).reshaped(_shapeY.nodes, _shapeX.units);
				Eigen::Index maxRow;
				mapLambda.col(winUnitX).maxCoeff(&maxRow);
				{
					int winUnitY = winY(maxRow, 0);
					auto map = Eigen::Map<MatrixCm<float_t>>(_cpt->col(col).data() + static_cast<Eigen::Index>(maxRow * _kCellSize), _shapeY.units, _shapeX.units);
					auto mapDelta = Eigen::Map<MatrixCm<float_t>>(_deltaCpt->col(col).data() + static_cast<Eigen::Index>(maxRow * _kCellSize), _shapeY.units, _shapeX.units);
					mapDelta(winUnitY, winUnitX) += learningRate * (1 - map(winUnitY, winUnitX));
				}
			}
		}

		void cellWta(
			MatrixRm<int32_t>& winX,
			MatrixRm<int32_t>& winY,
			MatrixCm<float_t>& lambda)
		{
			// 中間層を想定したWTAによるCPT増分積算
			// セルごとに

			for (int x = 0; x < _shapeX.nodes; x++)
			{
				// 上位層から見て学習対象になるCPTセルを決定
				auto map = lambda.col(x).reshaped(_shapeY.nodes, _shapeX.units);
				MatrixCm<float_t>::Index row, col;
				map.col(winX(x, 0)).maxCoeff(&row, &col); // ←rowに学習対象のCPT
				// 学習対象のrow, x, winY(row), winX(x)
				int r = static_cast<int>(row);
				auto a = Eigen::Map<MatrixCm<float_t>>(_deltaCpt->col(x).data() + static_cast<int>(_kCellSize * row), _shapeY.units, _shapeX.units);
				a(winY(r), winX(x)) += 1.f;
			}
		}

		void wtaDown(
			MatrixRm<int32_t>& winX,
			MatrixRm<float_t>& belY,
			MatrixCm<float_t>& lambda)
		{
			// 上位層：WTA
			// 下位層：ベクトル

			for (int x = 0; x < _shapeX.nodes; x++)
			{
				// 上位層から見て学習対象になるCPTセルを決定
				auto map = lambda.col(x).reshaped(_shapeY.nodes, _shapeX.units);
				MatrixCm<float_t>::Index row, col;
				map.col(winX(x, 0)).maxCoeff(&row, &col); // ←rowに学習対象のCPT
				// 学習対象のrow, x, winY(row), winX(x)
				int r = static_cast<int>(row);
				auto a = Eigen::Map<MatrixCm<float_t>>(_deltaCpt->col(x).data() + static_cast<int>(_kCellSize * row), _shapeY.units, _shapeX.units);
				a.col(winX(x)).array() += belY.row(r).array();
			}
		}

		void addDelta()
		{
			// CPTの増分を足す

			_cpt->array() += _deltaCpt->array();
			_deltaCpt->setZero();
		}
		// コピー禁止・ムーブ禁止
	public:
		GridCpt(const GridCpt&) = delete;
		GridCpt(GridCpt&&) = delete;
		GridCpt& operator =(const GridCpt&) = delete;
		GridCpt& operator =(GridCpt&&) = delete;
	};

}

#endif // !_MONJU_GRID_CPT_H__
