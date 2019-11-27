#include "GpuDeviceContext.h"

// コールバック関数
void CL_CALLBACK notify_error(const char* errinfo, const void* private_info, size_t cb, void* user_data)
{
	std::cerr << (errinfo != nullptr ? errinfo : "") << std::endl;
}


monju::GpuDeviceContext::GpuDeviceContext()
{
	_context = nullptr;
	_platform_id = nullptr;;
}


monju::GpuDeviceContext::~GpuDeviceContext()
{
	release();
}

monju::GpuDeviceContext::GpuDeviceContext(const GpuDeviceContext & o)
{
}

monju::GpuDeviceContext& monju::GpuDeviceContext::operator=(const monju::GpuDeviceContext& o)
{
	return *this;
}

// プラットフォームをヒューリスティックに取得したいが、方法が思いつかないので最初のプラットフォームを取得

cl_platform_id monju::GpuDeviceContext::_findPlatform(int platform_idx)
{
	cl_uint num_platforms = 0;
	cl_platform_id platforms_buff[2];
	clGetPlatformIDs(2, platforms_buff, &num_platforms);
	if (platform_idx >= static_cast<int>(num_platforms))
		throw MonjuException();
	// プラットフォーム選択
	return platforms_buff[platform_idx];
}

// デバイスをヒューリスティックに取得したいが、方法が思いつかないので最初のデバイスを取得
// GPU１個であれば問題ないが、複数のGPUを搭載する場合は複数のデバイスを取得するようにする。

std::vector<cl_device_id> monju::GpuDeviceContext::_listDevices(cl_platform_id platform_id)
{
	cl_uint num_devices = 0;
	cl_device_id devices_buff[4];
	clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 4, devices_buff, &num_devices);
	return std::vector<cl_device_id>(devices_buff, devices_buff + num_devices);
}

void monju::GpuDeviceContext::_releaseDevices(std::vector<cl_device_id>* p_device_id_vec)
{
	for (cl_device_id p : *p_device_id_vec)
		clReleaseDevice(p);
	p_device_id_vec->clear();
}

cl_context monju::GpuDeviceContext::_createContext(cl_platform_id platform_id, std::vector<cl_device_id>& devices)
{
	cl_int errorcode;
	cl_context context = clCreateContext(nullptr, static_cast<cl_uint>(devices.size()), devices.data(), notify_error, this, &errorcode);
	if (errorcode != CL_SUCCESS)
		throw MonjuException();
	return context;
}

std::vector<monju::Device*> monju::GpuDeviceContext::_allocDeviceVec(cl_context context, std::vector<cl_device_id> device_id_vec)
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

void monju::GpuDeviceContext::_freeDeviceVec(std::vector<Device*>* p_device_vec)
{
	for (Device* p : *p_device_vec)
		p->release();
	p_device_vec->clear();
}

void monju::GpuDeviceContext::create(int platform_idx)
{
	_platform_id = _findPlatform(platform_idx);
	_device_id_vec = _listDevices(_platform_id);
	_context = _createContext(_platform_id, _device_id_vec);
	_device_vec = _allocDeviceVec(_context, _device_id_vec);
}

void monju::GpuDeviceContext::release()
{
	_freeDeviceVec(&_device_vec);
	if (_context != nullptr)
	{
		clReleaseContext(_context);
		_context = nullptr;
	}
	_device_id_vec.clear();
	_releaseDevices(&_device_id_vec);
	_platform_id = nullptr;
}

cl_platform_id monju::GpuDeviceContext::getPlatformId() const
{
	return _platform_id;
}

cl_context monju::GpuDeviceContext::getContext() const
{
	return _context;
}

int monju::GpuDeviceContext::numDevices() const
{
	return static_cast<int>(_device_vec.size());
}

monju::Device& monju::GpuDeviceContext::getDevice(int idx) const
{
	return *(_device_vec.at(static_cast<size_t>(idx)));
}
