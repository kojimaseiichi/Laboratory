#pragma once
#ifndef _MONJU_CONCURRENCY_CONTEXT_H__
#define _MONJU_CONCURRENCY_CONTEXT_H__

#include "mtrebi/ThreadPool.h"
#include "Closable.h"

namespace monju {

	class ConcurrencyContext : public Closable
	{
	private:
		std::unique_ptr<ThreadPool> _ptp;

	public:
		ConcurrencyContext(const ConcurrencyContext&) = delete;
		ConcurrencyContext& operator=(const ConcurrencyContext&) = delete;
	public:

		ConcurrencyContext()
		{
			_ptp.reset();
		}

		ConcurrencyContext(int max_threads)
		{
			_ptp = std::make_unique<ThreadPool>(max_threads);
		}
		void create(int max_threads)
		{
			_ptp = std::make_unique<ThreadPool>(max_threads);
		}
		~ConcurrencyContext()
		{
			close();
		}

		void close()
		{
			if (_ptp != nullptr)
			{
				_ptp->shutdown();
				_ptp.reset();
			}
		}

		ThreadPool& threadPool() const
		{
			return *_ptp;
		}
	};
}

#endif // !_MONJU_CONCURRENCY_CONTEXT_H__

