#pragma once
#ifndef _MONJU_DEVICE_H__
#define _MONJU_DEVICE_H__

#include "MonjuTypes.h"

namespace monju {

	// ΫL·ιOpenCL\[X
	class Device
	{
	private:
		// OΉ
		cl_context			_context;
		cl_device_id		_device_id;
		// ΣCΝΝiπϊ\θj
		cl_command_queue	_command_queue;

		// Rs[Φ~E[uΦ~
	public:
		Device(const Device&) = delete;
		Device(Device&&) = delete;
		Device& operator=(const Device&) = delete;
		Device& operator=(Device&&) = delete;

		// vpeB
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
