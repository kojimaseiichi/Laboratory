#include "OpenClException.h"
#include "_ClCommandQueue.h"
#include "_ClPlatformId.h"

cl_command_queue monju::_ClCommandQueue::_create_command_queue(cl_context context, cl_device_id deviceId)
{
	cl_int error;
	cl_command_queue commandQueue = clCreateCommandQueueWithProperties(
		context,
		_deviceId,
		nullptr,
		&error);
	if (error != CL_SUCCESS)
		throw OpenClException(error, "clCreateCommandQueueWithProperties");
	return commandQueue;
}

void monju::_ClCommandQueue::_release_command_queue()
{
	if (_commandQueue == nullptr)
		return;
	cl_int error = clReleaseCommandQueue(_commandQueue);
	if (error != CL_SUCCESS)
		throw OpenClException(error, "clReleaseCommandQueue");
	_commandQueue = nullptr;
}

void monju::_ClCommandQueue::_flush()
{
	cl_int error = clFlush(_commandQueue);
	if (error != CL_SUCCESS)
		throw OpenClException(error, "clFlush");
}

void monju::_ClCommandQueue::_finish()
{
	cl_int error = clFinish(_commandQueue);
	if (error != CL_SUCCESS)
		throw OpenClException(error, "clFinish");
}

monju::_ClCommandQueue::_ClCommandQueue(cl_context context, cl_device_id deviceId)
{
	_context = context;
	_deviceId = deviceId;
	_commandQueue = _create_command_queue(context, deviceId);
}

monju::_ClCommandQueue::~_ClCommandQueue()
{
	_release_command_queue();
}

cl_command_queue monju::_ClCommandQueue::clCommandQueue() const
{
	return _commandQueue;
}

void monju::_ClCommandQueue::flush()
{
	_flush();
}

void monju::_ClCommandQueue::finish()
{
	_finish();
}
