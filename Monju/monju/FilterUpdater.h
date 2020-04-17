#pragma once
#ifndef _MONJU_FILTER_UPDATER_H__
#define _MONJU_FILTER_UPDATER_H__

#include "MonjuTypes.h"

namespace monju
{
	class FilterUpdater
	{
	private:
		std::weak_ptr<MatrixCm<float_t>> _filter;
		std::unique_ptr<MatrixCm<float_t>> _delta;

	public:
		FilterUpdater()
		{

		}
		// コピー禁止・ムーブ禁止
	public:
		FilterUpdater(const FilterUpdater&) = delete;
		FilterUpdater(FilterUpdater&&) = delete;
		FilterUpdater& operator =(const FilterUpdater&) = delete;
		FilterUpdater& operator =(FilterUpdater&&) = delete;
	};
}

#endif // _MONJU_FILTER_UPDATER_H__