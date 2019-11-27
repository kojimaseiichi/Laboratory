#pragma once
#ifndef _MONJU_COMPUTE_RESOURCE_H__
#define _MONJU_COMPUTE_RESOURCE_H__

#include <vector>
#include <atomic>
#include <iostream>
#include <CL/cl.h>
#include <crtdbg.h>
#include "MonjuException.h"
#include "Device.h"

namespace monju {

	/// <summary>OpenCL�̌v�Z�������擾</summary>
	class GpuDeviceContext
	{
	private:
		cl_platform_id				_platform_id;
		std::vector<cl_device_id>	_device_id_vec;
		cl_context					_context;
		std::vector<Device*>		_device_vec;

		// �����E������
	public:
		GpuDeviceContext();
		~GpuDeviceContext();

		// �R�s�[�֎~
	private:
		GpuDeviceContext(const GpuDeviceContext& o);
		GpuDeviceContext& operator=(const GpuDeviceContext& o);

	private:
		// �v���b�g�t�H�[�����q���[���X�e�B�b�N�Ɏ擾���������A���@���v�����Ȃ��̂ōŏ��̃v���b�g�t�H�[�����擾
		cl_platform_id				_findPlatform(int platform_idx);
		// �f�o�C�X���q���[���X�e�B�b�N�Ɏ擾���������A���@���v�����Ȃ��̂ōŏ��̃f�o�C�X���擾
		// GPU�P�ł���Ζ��Ȃ����A������GPU�𓋍ڂ���ꍇ�͕����̃f�o�C�X���擾����悤�ɂ���B
		std::vector<cl_device_id>	_listDevices(cl_platform_id platform_id);
		void						_releaseDevices(std::vector<cl_device_id>* p_device_id_vec);
		cl_context					_createContext(cl_platform_id platform_id, std::vector<cl_device_id>& device_id_vec);
		std::vector<Device*>		_allocDeviceVec(cl_context context, std::vector<cl_device_id> device_id_vec);
		void						_freeDeviceVec(std::vector<Device*>* p_device_vec);


	public:
		void						create(int platform_idx);
		void						release();

		// �v���p�e�B
	public:
		cl_platform_id				getPlatformId() const;
		cl_context					getContext() const;
		int							numDevices() const;
		Device&						getDevice(int idx) const;
	};

} // namespace monju

#endif // _MONJU_COMPUTE_RESOURCE_H__