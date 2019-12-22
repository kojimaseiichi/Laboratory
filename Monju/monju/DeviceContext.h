#pragma once
#ifndef _MONJU_DEVICE_CONTEXT_H__
#define _MONJU_DEVICE_CONTEXT_H__

#include <vector>
#include <atomic>
#include <iostream>
#include <CL/cl.h>
#include <crtdbg.h>
#include "MonjuException.h"
#include "Device.h"

namespace monju {

	// �v�Z�f�o�C�X�̃R���e�L�X�g
	// ���OpenCL�v���b�g�t�H�[����ێ�
	// ������OpenCL�f�o�C�X�����L
	class DeviceContext
	{
	private:
		cl_platform_id				_platform_id;
		std::vector<cl_device_id>	_device_id_vec;
		cl_context					_context;
		std::vector<Device*>		_device_vec;

		// �����E������
	public:
		DeviceContext();
		~DeviceContext();

		// �R�s�[�֎~
	private:
		DeviceContext(const DeviceContext& o) = delete;
		DeviceContext& operator=(const DeviceContext& o) = delete;

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
		// OpenCL�v���b�g�t�H�[��������
		// �v���b�g�t�H�[��ID�̓V�X�e���ŗL�ŊO������^������
		void						create(int platform_idx);
		// OpenCL�v���b�g�t�H�[�����
		void						release();

		// �v���p�e�B
	public:
		// OpenCL�v���b�g�t�H�[��ID
		cl_platform_id				getPlatformId() const;
		// OpenCL�R���e�L�X�g�i�����̃f�o�C�X���܂ށj
		cl_context					getContext() const;
		// �f�o�C�X��
		int							numDevices() const;
		// �f�o�C�X���擾�iGPU�P�{�[�h�j
		Device&						getDevice(int idx) const;
	};

} // namespace monju

#endif // _MONJU_DEVICE_CONTEXT_H__