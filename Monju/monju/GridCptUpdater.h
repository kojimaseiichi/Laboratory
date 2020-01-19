#pragma once
#ifndef _MONJU_GRID_CPT_UPDATER_H__
#define _MONJU_GRID_CPT_UPDATER_H__
#include "MonjuTypes.h"

namespace monju {

	class GridCptUpdater
	{
	private:
		const int
			_kNodesX,
			_kUnitsPerNodeX,
			_kNodesY,
			_kUnitsPerNodeY;
		MatrixCm<float_t> 
			_deltaGridCpt,
			_cacheLambda;
		const int _kCellSize;

	public:
		GridCptUpdater(
			int nodesX,
			int unitsPerNodeX,
			int nodesY,
			int unitsPerNodeY
		):
			_kNodesX(nodesX),
			_kUnitsPerNodeX(unitsPerNodeX),
			_kNodesY(nodesY),
			_kUnitsPerNodeY(unitsPerNodeY),
			_kCellSize(unitsPerNodeX* unitsPerNodeY)
		{
			const int rows = _kCellSize * nodesX;
			_deltaGridCpt.resize(rows, nodesY);
			_deltaGridCpt.setZero();
		}

		void wtaDown(
			MatrixRm<int32_t>& winX,
			MatrixRm<int32_t>& winY,
			MatrixCm<float_t>& lambda)
		{
			// 上位層：WTA
			// 下位層：WTA

			for (int x = 0; x < _kNodesX; x++)
			{
				// 上位層から見て学習対象になるCPTセルを決定
				auto map = lambda.col(x).reshaped(_kNodesY, _kUnitsPerNodeX);
				MatrixCm<float_t>::Index row, col;
				map.col(winX(x, 0)).maxCoeff(&row, &col); // ←rowに学習対象のCPT
				// 学習対象のrow, x, winY(row), winX(x)
				int r = static_cast<int>(row);
				auto a = Eigen::Map<MatrixCm<float_t>>(_deltaGridCpt.col(x).data() + _kCellSize * static_cast<int>(row), _kUnitsPerNodeY, _kUnitsPerNodeX);
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

			for (int x = 0; x < _kNodesX; x++)
			{
				// 上位層から見て学習対象になるCPTセルを決定
				auto map = lambda.col(x).reshaped(_kNodesY, _kUnitsPerNodeX);
				MatrixCm<float_t>::Index row, col;
				map.col(winX(x, 0)).maxCoeff(&row, &col); // ←rowに学習対象のCPT
				// 学習対象のrow, x, winY(row), winX(x)
				int r = static_cast<int>(row);
				auto a = Eigen::Map<MatrixCm<float_t>>(_deltaGridCpt.col(x).data() + _kCellSize * static_cast<int>(row), _kUnitsPerNodeY, _kUnitsPerNodeX);
				a.col(winX(x)).array() += belY.row(r).array();
			}
		}

		void addDelta(MatrixCm<float_t>& cpt, float_t learningRate)
		{
			// 蓄積したcptの差分を反映し、正規化する
			
			cpt.array() += _deltaGridCpt.array() * learningRate;
			_deltaGridCpt.setZero();
			for (int x = 0; x < _kNodesX; x++)
			{
				for (int y = 0; y < _kNodesY; y++)
				{
					auto a = Eigen::Map<MatrixCm<float_t>>( cpt.data() + (x * _kNodesY + y) * _kCellSize, _kUnitsPerNodeY, _kUnitsPerNodeX);
					a.array().rowwise() /= a.colwise().sum().array();
				}
			}
		}


	};

}

#endif // !_MONJU_GRID_CPT_UPDATER_H__
