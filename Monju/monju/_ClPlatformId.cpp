#include "_ClPlatformId.h"

void monju::_ClPlatformId::_getPlatformIds()
{
	cl_platform_id platforms[100];
	cl_uint retNum;
	cl_int error = clGetPlatformIDs(100, platforms, &retNum);
	if (error != CL_SUCCESS)
		throw OpenClException(error, "clGetPlatformIDs");

	_platformIds.clear();
	for (int n = 0; n < retNum; n++)
		_platformIds.push_back(platforms[n]);
}

void monju::_ClPlatformId::_getDeviceIds(cl_platform_id platform)
{
	cl_device_id devices[100];
	cl_uint retNum;
	cl_int error = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 100, devices, &retNum);
	if (error != CL_SUCCESS)
		throw OpenClException(error, "clGetDeviceIDs");
	for (int n = 0; n < retNum; n++)
		_deviceIds.push_back(devices[n]);
}

monju::_ClPlatformId::_ClPlatformId(int selectPlatform)
{
	_getPlatformIds();
	if (selectPlatform < 0 || selectPlatform >= _platformIds.size())
		throw MonjuException("selectPlatform‚ª”ÍˆÍŠO");
	selectedPlatformId = static_cast<size_t>(selectPlatform);

	_getDeviceIds(_platformIds.at(selectedPlatformId));
}

monju::_ClPlatformId::~_ClPlatformId()
{
}

std::vector<cl_platform_id> monju::_ClPlatformId::clPlatformIds() const
{
	return _platformIds;
}

cl_platform_id monju::_ClPlatformId::clPlatformId() const
{
	return _platformIds.at(selectedPlatformId);
}

std::vector<cl_device_id> monju::_ClPlatformId::clDeviceIds() const
{
	return _deviceIds;
}
