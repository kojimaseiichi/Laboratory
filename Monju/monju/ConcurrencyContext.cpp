#include "ConcurrencyContext.h"
#include "mtrebi/ThreadPool.h"

monju::ConcurrencyContext::ConcurrencyContext()
{
	_ptp.reset();
}

monju::ConcurrencyContext::ConcurrencyContext(int max_threads)
{
	_ptp = std::make_unique<ThreadPool>(max_threads);
}

void monju::ConcurrencyContext::create()
{
	_ptp->init();
}

void monju::ConcurrencyContext::close()
{
	if (_ptp != nullptr)
	{
		_ptp->shutdown();
		_ptp.reset();
	}
}

monju::ThreadPool& monju::ConcurrencyContext::threadPool() const
{
	return *_ptp;
}

monju::ConcurrencyContext::~ConcurrencyContext()
{
	close();
}
