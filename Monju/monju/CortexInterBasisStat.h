#pragma once
#ifndef _MONJU_CORTEX_INTER_BASIS_STAT_H__
#define _MONJU_CORTEX_INTER_BASIS_STAT_H__

#include "Closable.h"
#include "GridMatrixStorage.h"
#include "ConcurrencyContext.h"

namespace monju {

	// 基底間の統計データをファイルに保存
	class CortexInterBasisStat : public Closable
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


	};
}

#endif // !_MONJU_CORTEX_INTER_BASIS_STAT_H__

