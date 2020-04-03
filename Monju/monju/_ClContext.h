#pragma once
#ifndef _MONJU__CL_CONTEXT_H__
#define _MONJU__CL_CONTEXT_H__

#include "MonjuTypes.h"

namespace monju {

	void CL_CALLBACK cl_context_notify_error(const char* errinfo, const void* private_info, size_t cb, void* user_data);

	class _ClContext
	{
	private:
		std::vector<cl_device_id> _deviceIds;
		cl_context _context; // 解放する

		void _createContext(std::vector<cl_device_id>& deviceIds);
		void _releaseContext();

	public:
		_ClContext(const std::vector<cl_device_id> deviceIds);
		~_ClContext();
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

