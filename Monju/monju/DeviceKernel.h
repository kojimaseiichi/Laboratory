#pragma once

#ifndef _MONJU_DEVICE_KERNEL_H__
#define _MONJU_DEVICE_KERNEL_H__

#include <string>
#include <map>
#include <CL/cl.h>
#include "DeviceContext.h"
#include "DeviceProgram.h"
#include "Device.h"
#include "OpenClException.h"
#include "util_str.h"

namespace monju {

	// OpenCL�̃J�[�l�����b�p�[��{�N���X
	// �J�[�l���\�[�X�̃e���v���[�g�ϐ��̔��f�A�R���p�C���A���s���Ǘ�
	class DeviceKernel
	{
		// �����E�������E�j��
	public:
		DeviceKernel()
		{
			_p_device = nullptr;
			_p_program = nullptr;
			_kernel = nullptr;
			_dim = 0;
		}
		virtual ~DeviceKernel();

		// �R�s�[�֎~
	protected:
		DeviceKernel(const DeviceKernel&) = delete;
		DeviceKernel& operator=(const DeviceKernel&) = delete;

	protected:
		// �O���ˑ����\�[�X
		Device*			_p_device;
		DeviceProgram*	_p_program;
		std::string		_kernel_name;
		cl_kernel		_kernel;
		cl_int			_dim;
		std::vector<size_t>	_global_work_size;
		std::vector<size_t> _local_work_size;

	protected:
		// �R���p�C���ς݃J�[�l���v���O�������f�o�C�X�ɔz�u���Ď��s�\�ȏ�ԂɑJ��
		cl_kernel			_createKernel(cl_program program, std::string kernel_name, std::map<std::string, std::string>& params)
		{
			std::string parameterized_kernel_name = util_str::parameterizePlaceholders(kernel_name, params);
			cl_int error_code;
			cl_kernel kernel = clCreateKernel(program, parameterized_kernel_name.c_str(), &error_code);
			if (error_code != CL_SUCCESS)
				throw OpenClException(error_code);
			return kernel;
		}
		// �J�[�l�������s�\�ȏ�ԂɑJ�ځi_compileProgram�A_createKernel�j
		void				_initKernel(cl_device_id device_id, cl_program program, std::string kernel_name, std::map<std::string, std::string>& params)
		{
			_kernel = _createKernel(program, kernel_name, params);
		}
		// �v���O�����R���p�C��(CL�t�@�C��)�A�J�[�l������
		void				_create(
			Device& device,
			DeviceProgram& program,
			std::string kernel_name,
			std::map<std::string, std::string>& params,
			std::vector<size_t>& global_work_size,
			std::vector<size_t>& local_work_size)
		{
			_p_device = &device;
			_p_program = &program;
			_kernel_name = kernel_name;
			_global_work_size = global_work_size;
			_local_work_size = _local_work_size;
			_initKernel(device.getClDeviceId(), program.program(), kernel_name, params);
		}
		// �f�o�C�X��ɔz�u�ς݂̃J�[�l���v���O���������s
		// �����͎��O�ɐݒ肵�Ă����K�v������
		void				_run()
		{
			cl_int error_code = clEnqueueNDRangeKernel(
				_p_device->getClCommandQueue(),	// OpenCL�L���[
				_kernel,						// �J�[�l��
				_dim,							// 2����
				nullptr,						// global work offset
				_global_work_size.data(),		// global work size
				_local_work_size.data(),		// local work size
				0,								// number of events in wait list
				nullptr,						// event wait list
				nullptr);						// event
			if (error_code != CL_SUCCESS)
				throw OpenClException(error_code);
		}

		// �v���p�e�B
	public:
		Device&				getDevice() const { return *_p_device; }
		DeviceProgram&		getProgram() const { return *_p_program; }
		std::string			getKernelName() const { return _kernel_name; }
		cl_kernel			getKernel() const { return _kernel; }
		std::vector<size_t>	globalWorkSize() const { return _global_work_size; }
		std::vector<size_t>	localWorkSize() const { return _local_work_size; }

		// ���J�֐�
	public:
		// OpenCL�J�[�l������
		void		create(
			Device& device,
			DeviceProgram& program,
			std::string kernel_name,
			std::map<std::string, std::string>& params,
			std::vector<size_t>& global_work_size,
			std::vector<size_t>& local_work_size
		)
		{
			if (global_work_size.size() != local_work_size.size())
				throw MonjuException("global_work_size��local_work_size�̎�������v���Ȃ�");
			_create(device, program, kernel_name, params, global_work_size, local_work_size);
		}
		void		release()
		{
			if (_kernel != nullptr)
			{
				cl_int error_code = clReleaseKernel(_kernel);
				if (error_code != CL_SUCCESS)
					throw OpenClException(error_code);
				_kernel = nullptr;
			}
			_p_device = nullptr;
			_p_program = nullptr;
			_kernel_name = "";
			_dim = 0;
			_global_work_size.clear();
			_local_work_size.clear();
		}
		void		compute(
			std::vector<size_t>& global_work_size
		);
		void		compute(
			std::vector<size_t>& global_work_size,
			std::vector<size_t>& local_work_size
		)
		{
			_run();
		}

	}; // class DeviceKernel
} // namespace monju

#endif // _MONJU_KERNEL_BASE_H__