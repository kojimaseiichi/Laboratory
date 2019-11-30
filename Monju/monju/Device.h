#pragma once
#ifndef _MONJU_DEVICE_H__
#define _MONJU_DEVICE_H__

#include <CL/cl.h>

namespace monju {

	/// <summary>計算デバイス<br>
	/// GPUボード１枚分<br>
	/// OpenCLのラッパー</summary>
	class Device
	{
	private:
		cl_context			_context;
		cl_device_id		_device_id;
		cl_command_queue	_command_queue;

		// 初期化・生成
	public:
		Device();
		~Device();

		// コピー禁止
	private:
		Device(const Device& o) = delete;
		Device& operator=(const Device& o) = delete;

	private:
		// 初期化
		cl_command_queue	_createCommandQueue(cl_context context, cl_device_id device_id);

		// プロパティ
	public:
		cl_context			getContext() const { return _context; }
		cl_device_id		getDeviceId() const	{ return _device_id; }
		cl_command_queue	getCommandQueue() const { return _command_queue; }

	public:
		// OpenCLデバイス初期化
		void				create(cl_context context, cl_device_id device_id);
		// OpenCLデバイス解放
		void				release();

	};

} // namespace monju
#endif // !_MONJU_DEVICE_H__
