#pragma once
#ifndef _MONJU_CL_MACHINE_H__
#define _MONJU_CL_MACHINE_H__

#include "_ClPlatformId.h"
#include "_ClContext.h"
#include "_ClCommandQueue.h"
#include <memory>

namespace monju
{
	/*
	platformId�Ŋe�}�V����OpenCL������I������B
	*/
	class ClMachine
	{
	private:
		int _platformId;
		std::shared_ptr<_ClPlatformId> _clPlatformId;	// ����\��
		std::shared_ptr<_ClContext> _clContext;			// ����\��

		void _createPlatformId();
		void _createContext();
		void _releasePlatformId();
		void _releaseContext();

	public:
		ClMachine(int platformId);
		~ClMachine();
		std::weak_ptr<_ClPlatformId> clPlatformId() const;
		std::weak_ptr<_ClContext> clContext() const;
		std::vector<cl_device_id> deviceIds() const;

		// �R�s�[�֎~�E���[�u�֎~
	public:
		ClMachine(const ClMachine&) = delete;
		ClMachine(ClMachine&&) = delete;
		ClMachine& operator=(const ClMachine&) = delete;
		ClMachine& operator=(ClMachine&&) = delete;
	};
}

#endif // !_MONJU_CL_MACHINE_H__

