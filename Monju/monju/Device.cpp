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

void monju::Device::create(cl_context context, cl_device_id device_id)
{
	_context = context;
	_device_id = device_id;

	cl_int error_code;
	_command_queue = clCreateCommandQueueWithProperties(context, device_id, nullptr, &error_code);
	if (error_code != CL_SUCCESS)
		throw OpenClException(error_code);

}

void monju::Device::release()
{
	if (_command_queue != nullptr)
	{
		cl_int error_code = clReleaseCommandQueue(_command_queue);
		if (error_code != CL_SUCCESS)
			throw OpenClException(error_code);
		_command_queue = nullptr;
	}
	_context = nullptr;
	_device_id = nullptr;
}
