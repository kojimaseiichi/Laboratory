#pragma once
#ifndef _MONJU_CONTINGENCY_TABLE_STORAGE_H__
#define _MONJU_CONTINGENCY_TABLE_STORAGE_H__

#include <vector>
#include "Closable.h"
#include "Environment.h"
#include "GridMatrixStorage.h"
#include "ConcurrencyContext.h"
#include "Synchronizable.h"
#include "Extent.h"
#include "util_file.h"

namespace monju {

	// 相関の勝者ユニットの分割表
	// CPTを計算
	class ContingencyTableStorage : GridMatrixStorage
	{
	private:
		GridExtent _gridExtent;

	public:/*コンストラクタ*/
		ContingencyTableStorage(std::string fileName, LayerShape x, LayerShape y)
			: GridMatrixStorage(fileName)
		{
			_gridExtent.cross(x, y);
		}
		~ContingencyTableStorage()
		{

		}

	public:/*公開メンバ*/
		void increment(std::weak_ptr<MatrixRm<int32_t>> winX, std::weak_ptr<MatrixRm<int32_t>> winY)
		{
			auto px = winX.lock();
			auto py = winY.lock();
			auto ax = _toVector(*px);
			auto ay = _toVector(*py);
			this->coeffOp<int32_t>(
					"ct",
					ax,
					ay,
					[](const int32_t v)->int32_t {
						return v + 1;
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
			this->readGrid("ct", count);
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

	private:/*ヘルパ*/
		void _prepareStorage()
		{
			this->prepare<int32_t>("ct", _gridExtent, kDensityRectangular, kColMajor, kColMajor);
			this->prepare<float_t>("cpt", _gridExtent, kDensityRectangular, kColMajor, kColMajor);
		}
		std::vector<int32_t>&& _toVector(MatrixRm<int32_t>& win) const
		{
			std::vector<int32_t> v;
			for (int row = 0; row < win.rows(); row++)
				v.push_back(win(row, 0));
			return std::move(v);
		}

	};
}

#endif // !_MONJU_CONTINGENCY_TABLE_STORAGE_H__

