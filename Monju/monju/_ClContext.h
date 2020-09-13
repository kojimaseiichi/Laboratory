#pragma once
#ifndef _MONJU__CL_CONTEXT_H__
#define _MONJU__CL_CONTEXT_H__

#include <vector>
#include <CL/cl.h>

namespace monju {

	/* OpenCLエラー通知コールバック関数*/
	void CL_CALLBACK cl_context_notify_error(const char* errinfo, const void* private_info, size_t cb, void* user_data);

	class _ClContext
	{
	private:
		std::vector<cl_device_id> _deviceIds;
		cl_context _context; // 解放する

	private:
		void _create_context(std::vector<cl_device_id>& deviceIds);
		void _release_context();

	public:
		_ClContext(const std::vector<cl_device_id> deviceIds);
		~_ClContext();

	public:
		cl_context context() const;
		std::vector<cl_device_id> deviceIds() const;

		// コピー禁止・ムーブ禁止
	public:
		_ClContext(const _ClContext&) = delete;
		_ClContext(_ClContext&&) = delete;
		_ClContext& operator=(const _ClContext&) = delete;
		_ClContext& operator=(_ClContext&&) = delete;

	};
}

#endif // !_MONJU__CL_CONTEXT_H__

