#include "OpenClContext.h"

void monju::OpenClContext::_createPlatformId()
{
	_clPlatformId = std::make_shared<ClPlatformId>(_platformId);
}

void monju::OpenClContext::_createContext()
{
	_clContext = std::make_shared<ClContext>(_clPlatformId->allPlatforms());
}

void monju::OpenClContext::_createCommandQueue()
{
	std::vector<cl_device_id> deviceIds = _clContext->deviceIds;
	_clQueueArr.resize(deviceIds.size());
	for (int n = 0; n < deviceIds.size(); n++)
		_clQueueArr[n] = std::make_shared<ClCommandQueues>(_clContext->context(), deviceIds[n]);
}

void monju::OpenClContext::_releasePlatformId()
{
	bool unique = _clPlatformId.unique();
	_clPlatformId.reset();
	if (!unique)
		throw MonjuException("_clPlatformId");
}

void monju::OpenClContext::_releaseContext()
{
	bool unique = _clContext.unique();
	_clContext.reset();
	if (!unique)
		throw MonjuException("_clContext");
}

void monju::OpenClContext::_releaseCommandQueue()
{
	bool all = true;
	for (int n = 0; n < _clQueueArr.size(); n++)
	{
		all &= _clQueueArr[n].unique(); // ‚·‚×‚Äunique == true‚ª•K—v
		_clQueueArr[n].reset();
	}
	_clQueueArr.clear();
	if (!all)
		throw MonjuException("_clQueueArr");
}

monju::OpenClContext::OpenClContext(int platformId)
{
	_platformId = platformId;
	_createPlatformId();
	_createContext();
	_createCommandQueue();
}

monju::OpenClContext::~OpenClContext()
{
	_releaseCommandQueue();
	_releaseContext();
	_releasePlatformId();
}

std::weak_ptr<monju::ClPlatformId> monju::OpenClContext::clPlatformId() const
{
	return _clPlatformId;
}

std::weak_ptr<monju::ClContext> monju::OpenClContext::clContext() const
{
	return _clContext;
}

std::weak_ptr<monju::ClCommandQueues> monju::OpenClContext::clCommandQueue(int n) const
{
	return _clQueueArr[n];
}
