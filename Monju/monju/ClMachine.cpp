#include "ClMachine.h"

#include "MonjuException.h"

#include "_ClPlatformId.h"
#include "_ClContext.h"
#include "_ClCommandQueue.h"

void monju::ClMachine::_create_platform_id()
{
	_clPlatformId = std::make_shared<_ClPlatformId>(_platformId);
}

void monju::ClMachine::_create_context()
{
	_clContext = std::make_shared<_ClContext>(_clPlatformId->clDeviceIds());
}

void monju::ClMachine::_release_platform_id()
{
	bool unique = _clPlatformId.use_count() == 1;
	_clPlatformId.reset();
	if (!unique)
		throw MonjuException("clPlatformId");
}

void monju::ClMachine::_release_context()
{
	bool unique = _clContext.use_count() == 1;
	_clContext.reset();
	if (!unique)
		throw MonjuException("clContext");
}

monju::ClMachine::ClMachine(int platformId)
{
	_platformId = platformId;
	_create_platform_id();
	_create_context();
}

monju::ClMachine::~ClMachine()
{
	_release_context();
	_release_platform_id();
}

std::weak_ptr<monju::_ClPlatformId> monju::ClMachine::clPlatformId() const
{
	return _clPlatformId;
}

std::weak_ptr<monju::_ClContext> monju::ClMachine::clContext() const
{
	return _clContext;
}

std::vector<cl_device_id> monju::ClMachine::deviceIds() const
{
	return _clContext->deviceIds();
}

cl_device_id monju::ClMachine::defaultDeviceId() const
{
	if (_clContext->deviceIds().size() == 0)
		throw MonjuException();
	return _clContext->deviceIds().at(0);
}

