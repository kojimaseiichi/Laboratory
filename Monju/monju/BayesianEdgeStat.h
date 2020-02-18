#pragma once
#ifndef _MONJU_BAYESIAN_EDGE_STAT_H__
#define _MONJU_BAYESIAN_EDGE_STAT_H__

#include "Closable.h"
#include "GridMatrixStorage.h"
#include "ConcurrencyContext.h"

namespace monju {

	// 基底間の統計データをファイルに保存
	class BayesianEdgeStat
	{
	private:
		using TStorage = RectangularGridMatrixStorage<int32_t>;

		std::string _id;
		const int
			_kNodesX,
			_kUnitsPerNodeX,
			_kNodesY,
			_kUnitsPerNodeY;
		std::shared_ptr<TStorage> _storage;
		Synchronizable _synch;
		ConcurrencyContext _conc;

	public:
		std::future<void> accumulate(MatrixRm<int32_t>& winX, MatrixRm<int32_t>& winY)
		{
			MatrixRm<int32_t>* px = new MatrixRm<int32_t>(winX);
			MatrixRm<int32_t>* py = new MatrixRm<int32_t>(winY);
			const auto task = [=](std::weak_ptr<TStorage> sto, MatrixRm<int32_t>* px, MatrixRm<int32_t>* py) -> void
			{
				std::unique_ptr<MatrixRm<int32_t>> x(px);
				std::unique_ptr<MatrixRm<int32_t>> y(py);
				if (auto pSto = sto.lock())
				{
					// LOCK -----------------------------
					WriteGuard g(_synch);
					for (int row = 0; row < py->rows(); row++)
					{
						for (int col = 0; col < px->rows(); col++)
						{
							pSto->addElement(row, col, y->coeff(row, 0), x->coeff(col, 0), 1);
						}
					}
				}
			};
			return _conc.threadPool().submit(task, _storage, px, py);
		}

	};
}

#endif // !_MONJU_BAYESIAN_EDGE_STAT_H__

