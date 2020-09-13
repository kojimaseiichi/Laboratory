#pragma once
#ifndef _MONJU__CL_COMMAND_QUEUES_H__
#define _MONJU__CL_COMMAND_QUEUES_H__

#include <CL/cl.h>

namespace monju {

	class _ClCommandQueue
	{
	private:
		cl_context _context;
		cl_device_id _deviceId;
		cl_command_queue _commandQueue;	// 解放予定

	private:
		cl_command_queue _create_command_queue(cl_context context, cl_device_id deviceId);
		void _release_command_queue();
		void _flush();
		void _finish();

	public:
		_ClCommandQueue(cl_context context, cl_device_id deviceId);
		~_ClCommandQueue();

	public:
		cl_command_queue clCommandQueue() const;

	public:
		void flush();
		void finish();

		// コピー禁止・ムーブ禁止
	public:
		_ClCommandQueue(const _ClCommandQueue&) = delete;
		_ClCommandQueue(_ClCommandQueue&&) = delete;
		_ClCommandQueue& operator=(const _ClCommandQueue&) = delete;
		_ClCommandQueue& operator=(_ClCommandQueue&&) = delete;
	};

}

#endif // !_MONJU__CL_COMMAND_QUEUES_H__

