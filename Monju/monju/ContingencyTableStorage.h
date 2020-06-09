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
	class ContingencyTableStorage
	{
	private:
		std::shared_ptr<Environment> _env;
		std::string _id;
		GridExtent _gridExtent;
		std::shared_ptr<GridMatrixStorage> _storage;
		Synchronizable _synch;
		ConcurrencyContext _conc;

	public:/*コンストラクタ*/
		ContingencyTableStorage(std::weak_ptr<Environment> env, std::string id, LayerShape x, LayerShape y)
		{
			_env = env.lock();
			_id = id;
			_gridExtent.setCpt(x, y);
			_storage = std::make_shared<GridMatrixStorage>(_dataFileName());
		}
		~ContingencyTableStorage()
		{

		}

	public:/*公開メンバ*/
		std::future<void> add(std::weak_ptr<MatrixRm<int32_t>> winX, std::weak_ptr<MatrixRm<int32_t>> winY)
		{
			auto px = winX.lock();
			auto py = winY.lock();
			auto x = _toVector(*px);
			auto y = _toVector(*py);
			const auto task = [](Synchronizable& synch, std::shared_ptr<GridMatrixStorage> storage, std::unique_ptr<std::vector<int32_t>> ax, std::unique_ptr<std::vector<int32_t>> ay) -> void
			{
				// LOCK -----------------------------
				WriteGuard g(synch);
				for (int col = 0; col < ax->size(); col++)
				{
					for (int row = 0; row < ay->size(); row++)
						storage->coeffOp<int32_t>("ct", *ax, *ay,
							[](const int32_t v)->int32_t {return v + 1; });
				}
			};
			auto wrapper = [&]()
			{
				task(_synch, _storage, std::move(x), std::move(y));
			};
			return _conc.threadPool().submit(wrapper);
		}

	private:/*ヘルパ*/
		std::string _dataFileName() const
		{
			return util_file::combine(_env->info().workFolder, _id, "dbm");
		}
		void _prepareStorage()
		{
			_storage->prepare<int32_t>("ct", _gridExtent, kDensityRectangular, kColMajor, kColMajor);
		}
		std::unique_ptr<std::vector<int32_t>> _toVector(MatrixRm<int32_t>& win) const
		{
			auto v = std::make_unique<std::vector<int32_t>>();
			for (int row = 0; row < win.rows(); row++)
				v->push_back(win(row, 0));
			return v;
		}

	};
}

#endif // !_MONJU_CONTINGENCY_TABLE_STORAGE_H__

