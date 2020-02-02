#pragma once
#ifndef _MONJU_CL_COMMAND_QUEUES_H__
#define _MONJU_CL_COMMAND_QUEUES_H__

#include "ClPlatformId.h"

namespace monju {

	class ClCommandQueues
	{
	private:
		cl_context _context;
		cl_device_id _deviceId;
		cl_command_queue _commandQueue;	// 解放予定

		cl_command_queue _createCommandQueue(cl_context context, cl_device_id deviceId)
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
		void _releaseCommandQueue()
		{
			if (_commandQueue == nullptr)
				return;
			cl_int error = clReleaseCommandQueue(_commandQueue);
			if (error != CL_SUCCESS)
				throw OpenClException(error, "clReleaseCommandQueue");
			_commandQueue = nullptr;
		}
		void _flush()
		{
			cl_int error = clFlush(_commandQueue);
			if (error != CL_SUCCESS)
				throw OpenClException(error, "clFlush");
		}
		void _finish()
		{
			cl_int error = clFinish(_commandQueue);
			if (error != CL_SUCCESS)
				throw OpenClException(error, "clFinish");
		}

	public:
		ClCommandQueues(cl_context context, cl_device_id deviceId)
		{
			_context = context;
			_deviceId = deviceId;
			_commandQueue = _createCommandQueue(context, deviceId);
		}
		~ClCommandQueues()
		{
			_releaseCommandQueue();
		}
		cl_command_queue clCommandQueue() const
		{
			return _commandQueue;
		}
		void flush()
		{
			_flush();
		}
		void finish()
		{
			_finish();
		}

		// コピー禁止・ムーブ禁止
	public:
		ClCommandQueues(const ClCommandQueues&) = delete;
		ClCommandQueues(ClCommandQueues&&) = delete;
		ClCommandQueues& operator=(const ClCommandQueues&) = delete;
		ClCommandQueues& operator=(ClCommandQueues&&) = delete;
	};

}

#endif // !_MONJU_CL_COMMAND_QUEUES_H__

