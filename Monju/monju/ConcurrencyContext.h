#pragma once
#ifndef _MONJU_CONCURRENCY_CONTEXT_H__
#define _MONJU_CONCURRENCY_CONTEXT_H__

#include "MonjuTypes.h"
#include "Closable.h"

namespace monju {

	class ConcurrencyContext : public Closable
	{
	private:
		std::unique_ptr<ThreadPool> _ptp;

	public:
		ConcurrencyContext();
		ConcurrencyContext(int max_threads);
		~ConcurrencyContext();
		void create();
		void close();
		ThreadPool& threadPool() const;
		// コピー・ムーブ不可
	public:
		ConcurrencyContext(const ConcurrencyContext&) = delete;
		ConcurrencyContext(const ConcurrencyContext&&) = delete;
		ConcurrencyContext& operator=(const ConcurrencyContext&) = delete;
		ConcurrencyContext& operator=(const ConcurrencyContext&&) = delete;
	};
}

#endif // !_MONJU_CONCURRENCY_CONTEXT_H__

