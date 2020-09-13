#include "OpenClException.h"
#include "_ClContext.h"
#include "_ClPlatformId.h"

void CL_CALLBACK monju::cl_context_notify_error(const char* errinfo, const void* private_info, size_t cb, void* user_data)
{
}

void monju::_ClContext::_create_context(std::vector<cl_device_id>& deviceIds)
{
	cl_int error;
	_context = clCreateContext(
		nullptr,
		static_cast<cl_uint>(_deviceIds.size()),
		_deviceIds.data(),
		monju::cl_context_notify_error,	// コールバック関数
		nullptr,				// コールバックの引数
		&error);
	if (errno != CL_SUCCESS)
		throw OpenClException(error, "clCreateContext");
}

void monju::_ClContext::_release_context()
{
	if (_context == nullptr)
		return;
	cl_int error = clReleaseContext(_context);
	if (errno != CL_SUCCESS)
		throw OpenClException(error, "clReleaseContext");
	_context = nullptr;
}

monju::_ClContext::_ClContext(const std::vector<cl_device_id> deviceIds)
{
	_deviceIds = deviceIds;
	_create_context(_deviceIds);
}

monju::_ClContext::~_ClContext()
{
	_release_context();
}

cl_context monju::_ClContext::context() const
{
	return _context;
}

std::vector<cl_device_id> monju::_ClContext::deviceIds() const
{
	return _deviceIds;
}
