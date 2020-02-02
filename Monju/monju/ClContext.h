#pragma once
#ifndef _MONJU_CL_CONTEXT_H__
#define _MONJU_CL_CONTEXT_H__

#include <CL/cl.h>
#include "ClPlatformId.h"

namespace monju {

	void CL_CALLBACK callback_notify_error(const char* errinfo, const void* private_info, size_t cb, void* user_data);

	class ClContext
	{
	private:
		ClPlatformId* _pPlatformId;
		std::vector<cl_device_id> _deviceIds;
		cl_context _context; // �������

		cl_context _createContext(std::vector<cl_device_id>& deviceIds)
		{
			cl_int error;
			_context = clCreateContext(
				nullptr,
				_deviceIds.size(),
				_deviceIds.data(),
				callback_notify_error,	// �R�[���o�b�N�֐�
				nullptr,				// �R�[���o�b�N�̈���
				&error);
			if (errno != CL_SUCCESS)
				throw OpenClException(error, "clCreateContext");
		}

		void _releaseContext()
		{
			if (_context == nullptr)
				return;
			cl_int error = clReleaseContext(_context);
			if (errno != CL_SUCCESS)
				throw OpenClException(error, "clReleaseContext");
			_context = nullptr;
		}

	public:
		ClContext(ClPlatformId& platformId)
		{
			_pPlatformId = &platformId;
			_deviceIds = platformId.deviceIds();
			_context = _createContext(_deviceIds);
		}
		~ClContext()
		{
			_releaseContext();
		}
		cl_context context() const
		{
			return _context;
		}
		std::vector<cl_device_id> deviceIds() const
		{
			return _deviceIds;
		}

		// �R�s�[�֎~�E���[�u�֎~
	public:
		ClContext(const ClContext&) = delete;
		ClContext(ClContext&&) = delete;
		ClContext& operator=(const ClContext&) = delete;
		ClContext& operator=(ClContext&&) = delete;

	};
}

#endif // !_MONJU_CL_CONTEXT_H__

