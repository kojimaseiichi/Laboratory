#include "Device.h"


monju::Device::Device()
{
	_context = nullptr;
	_device_id = nullptr;
	_command_queue = nullptr;
}

monju::Device::~Device()
{
	_release();
}

void monju::Device::create(cl_context context, cl_device_id device_id)
{
	_command_queue = _clCreateCommandQueueWithProperties(context, device_id);
	_context = context;
	_device_id = device_id;
}

void monju::Device::flush()
{
	_clFlush();
}

void monju::Device::finish()
{
	_clFinish();
}

void monju::Device::release()
{
	_release();
	_context = nullptr;
	_device_id = nullptr;
}

void monju::Device::_release()
{
	_clReleaseCommandQueue();
}

cl_command_queue monju::Device::_clCreateCommandQueueWithProperties(cl_context context, cl_device_id device_id)
{
	cl_int error_code;
	_command_queue = clCreateCommandQueueWithProperties(context, device_id, nullptr, &error_code);
	if (error_code != CL_SUCCESS)
		throw OpenClException(error_code);
	return _command_queue;
}

void monju::Device::_clReleaseCommandQueue()
{
	if (_command_queue != nullptr)
	{
		cl_int error_code;
		error_code = clReleaseCommandQueue(_command_queue);
		if (error_code != CL_SUCCESS)
			throw OpenClException(error_code);
		_command_queue = nullptr;
	}
}

void monju::Device::_clFlush()
{
	if (_command_queue != nullptr)
	{
		cl_int error_code = clFlush(_command_queue);
		if (error_code != CL_SUCCESS)
			throw OpenClException(error_code);
	}
}

void monju::Device::_clFinish()
{
	if (_command_queue != nullptr)
	{
		cl_int error_code = clFinish(_command_queue);
		if (error_code != CL_SUCCESS)
			throw OpenClException(error_code);
	}
}

