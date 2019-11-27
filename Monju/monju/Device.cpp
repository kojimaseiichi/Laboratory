#include "Device.h"


monju::Device::Device()
{
	_context = nullptr;
	_device_id = nullptr;
	_command_queue = nullptr;
}

monju::Device::~Device()
{
}

cl_command_queue monju::Device::_createCommandQueue(cl_context context, cl_device_id device_id)
{
	cl_command_queue queue = clCreateCommandQueueWithProperties(context, device_id, 0, nullptr);
	return queue;
}

void monju::Device::create(cl_context context, cl_device_id device_id)
{
	_context = context;
	_device_id = device_id;
	_command_queue = clCreateCommandQueueWithProperties(context, device_id, NULL, 0);
}

void monju::Device::release()
{
	if (_command_queue != nullptr)
	{
		clReleaseCommandQueue(_command_queue);
		_command_queue = nullptr;
	}
	_context = nullptr;
	_device_id = nullptr;
}
