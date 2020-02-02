#pragma once
#ifndef _MONJU_DEVICE_H__
#define _MONJU_DEVICE_H__

#include <CL/cl.h>
#include "OpenClException.h"

namespace monju {

	// 保有するOpenCLリソース
	class Device
	{
	private:
		// 外部資源
		cl_context			_context;
		cl_device_id		_device_id;
		// 責任範囲（解放予定）
		cl_command_queue	_command_queue;

		// コピー禁止・ムーブ禁止
	public:
		Device(const Device&) = delete;
		Device(Device&&) = delete;
		Device& operator=(const Device&) = delete;
		Device& operator=(Device&&) = delete;

		// プロパティ
	public:
		cl_context			getClContext() const { return _context; }
		cl_device_id		getClDeviceId() const	{ return _device_id; }
		cl_command_queue	getClCommandQueue() const { return _command_queue; }

	public:
		Device();
		~Device();
		void	create(cl_context context, cl_device_id device_id);
		void	flush();
		void	finish();
		void	release();
	private:

		void	_release();
		cl_command_queue	_clCreateCommandQueueWithProperties(cl_context context, cl_device_id device_id);
		void	_clReleaseCommandQueue();
		void	_clFlush();
		void	_clFinish();
	};

} // namespace monju
#endif // !_MONJU_DEVICE_H__
