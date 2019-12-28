#include "DeviceContext.h"

// コールバック関数
void CL_CALLBACK notify_error(const char* errinfo, const void* private_info, size_t cb, void* user_data)
{
	std::cerr << (errinfo != nullptr ? errinfo : "") << std::endl;
}


monju::DeviceContext::DeviceContext()
{
	_context = nullptr;
	_platform_id = nullptr;;
}


monju::DeviceContext::~DeviceContext()
{
	release();
}

// プラットフォームをヒューリスティックに取得したいが、方法が思いつかないので最初のプラットフォームを取得

cl_platform_id monju::DeviceContext::_findPlatform(int platform_idx)
{
	cl_uint num_platforms = 0;
	cl_platform_id platforms_buff[2];
	cl_int error_code = clGetPlatformIDs(2, platforms_buff, &num_platforms);
	if (error_code != CL_SUCCESS)
		throw OpenClException(error_code);
	if (platform_idx >= static_cast<int>(num_platforms))
		throw MonjuException();
	// プラットフォーム選択
	return platforms_buff[platform_idx];
}

// デバイスをヒューリスティックに取得したいが、方法が思いつかないので最初のデバイスを取得
// GPU１個であれば問題ないが、複数のGPUを搭載する場合は複数のデバイスを取得するようにする。

std::vector<cl_device_id> monju::DeviceContext::_listDevices(cl_platform_id platform_id)
{
	cl_uint num_devices = 0;
	cl_device_id devices_buff[4];
	cl_int error_code = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 4, devices_buff, &num_devices);
	if (error_code != CL_SUCCESS)
		throw OpenClException(error_code);
	return std::vector<cl_device_id>(devices_buff, devices_buff + num_devices);
}

void monju::DeviceContext::_releaseDevices(std::vector<cl_device_id>* p_device_id_vec)
{
	for (cl_device_id p : *p_device_id_vec)
	{
		cl_int error_code = clReleaseDevice(p);
		if (error_code != CL_SUCCESS)
			throw OpenClException(error_code);
	}
	p_device_id_vec->clear();
}

cl_context monju::DeviceContext::_createContext(cl_platform_id platform_id, std::vector<cl_device_id>& devices)
{
	cl_int errorcode;
	cl_context context = clCreateContext(nullptr, static_cast<cl_uint>(devices.size()), devices.data(), notify_error, this, &errorcode);
	if (errorcode != CL_SUCCESS)
		throw MonjuException();
	return context;
}

std::vector<monju::Device*> monju::DeviceContext::_allocDeviceVec(cl_context context, std::vector<cl_device_id> device_id_vec)
{
	std::vector<Device*> devices;
	for (auto device_id : device_id_vec)
	{
		Device* p = new Device();
		p->create(_context, device_id);
		devices.push_back(p);
	}
	return devices;
}

void monju::DeviceContext::_freeDeviceVec(std::vector<Device*>* p_device_vec)
{
	for (Device* p : *p_device_vec)
		p->release();
	p_device_vec->clear();
}

void monju::DeviceContext::create(int platform_idx)
{
	_platform_id = _findPlatform(platform_idx);
	_device_id_vec = _listDevices(_platform_id);
	_context = _createContext(_platform_id, _device_id_vec);
	_device_vec = _allocDeviceVec(_context, _device_id_vec);
}

void monju::DeviceContext::release()
{
	_freeDeviceVec(&_device_vec);
	if (_context != nullptr)
	{
		cl_int error_code = clReleaseContext(_context);
		if (error_code != CL_SUCCESS)
			throw OpenClException(error_code);
		_context = nullptr;
	}
	_device_id_vec.clear();
	_releaseDevices(&_device_id_vec);
	_platform_id = nullptr;
}

cl_platform_id monju::DeviceContext::getPlatformId() const
{
	return _platform_id;
}

cl_context monju::DeviceContext::getContext() const
{
	return _context;
}

int monju::DeviceContext::numDevices() const
{
	return static_cast<int>(_device_vec.size());
}

monju::Device& monju::DeviceContext::getDevice(int idx) const
{
	return *(_device_vec.at(static_cast<size_t>(idx)));
}

// すべてのデバイスを取得

std::vector<monju::Device*> monju::DeviceContext::getAllDevices() const
{
	return _device_vec;
}
