#pragma once
#ifndef _MONJU_DEVICE_H__
#define _MONJU_DEVICE_H__

#include <CL/cl.h>
#include "OpenClException.h"

namespace monju {

	/// <summary>�v�Z�f�o�C�X<br>
	/// GPU�{�[�h�P����<br>
	/// OpenCL�̃��b�p�[</summary>
	class Device
	{
	private:
		cl_context			_context;
		cl_device_id		_device_id;
		cl_command_queue	_command_queue;

		// �������E����
	public:
		Device();
		~Device();

		// �R�s�[�֎~�E���[�u�֎~
	private:
		Device(const Device&) = delete;
		Device(Device&&) = delete;
		Device& operator=(const Device&) = delete;
		Device& operator=(Device&&) = delete;

		// �v���p�e�B
	public:
		cl_context			getClContext() const { return _context; }
		cl_device_id		getClDeviceId() const	{ return _device_id; }
		cl_command_queue	getClCommandQueue() const { return _command_queue; }

	public:
		// OpenCL�f�o�C�X������
		void				create(cl_context context, cl_device_id device_id);
		// OpenCL�f�o�C�X���
		void				release();

	};

} // namespace monju
#endif // !_MONJU_DEVICE_H__
