#pragma once

#ifndef _MONJU_DEVICE_KERNEL_H__
#define _MONJU_DEVICE_KERNEL_H__

#include "MonjuTypes.h"

namespace monju {

	// OpenCL�̃J�[�l�����b�p�[��{�N���X
	// �J�[�l���\�[�X�̃e���v���[�g�ϐ��̔��f�A�R���p�C���A���s���Ǘ�
	class DeviceKernel
	{
		// �����E�������E�j��
	public:
		DeviceKernel();
		virtual ~DeviceKernel();

	protected:
		// �O���ˑ����\�[�X
		DeviceProgram*	_p_program;
		std::string		_kernel_name;
		cl_kernel		_kernel;

	protected:
		// �R���p�C���ς݃J�[�l���v���O�������f�o�C�X�ɔz�u���Ď��s�\�ȏ�ԂɑJ��
		cl_kernel			_create_kernel(cl_program program, std::string kernel_name, param_map& params);
		// �J�[�l�������s�\�ȏ�ԂɑJ�ځi_compileProgram�A_createKernel�j
		void				_initKernel(cl_program program, std::string kernel_name, param_map& params);
		// �v���O�����R���p�C��(CL�t�@�C��)�A�J�[�l������
		void				_create(
			DeviceProgram& program,
			std::string kernel_name,
			param_map& params);
		// �f�o�C�X��ɔz�u�ς݂̃J�[�l���v���O���������s
		// �����͎��O�ɐݒ肵�Ă����K�v������
		void				_run(
			Device& device,
			std::vector<size_t>& global_work_size,
			std::vector<size_t>& local_work_size);

		void				_run(
			Device& device,
			std::vector<size_t>& global_work_size);

		// �v���p�e�B
	public:
		DeviceProgram&		getProgram() const { return *_p_program; }
		std::string			getKernelName() const { return _kernel_name; }
		cl_kernel			getKernel() const { return _kernel; }

		// ���J�֐�
	public:
		// OpenCL�J�[�l������
		void		create(
			DeviceProgram& program,
			std::string kernel_name,
			param_map& params
		);
		// OpenCL�J�[�l�����
		void		release();
		// �J�[�l���v�Z���s
		void		compute(
			Device& device,
			std::vector<size_t>& global_work_size
		);
		// �J�[�l���v�Z���s
		void		compute(
			Device& device,
			std::vector<size_t>& global_work_size,
			std::vector<size_t>& local_work_size
		);

		// �R�s�[�֎~�E���[�u�֎~
	protected:
		DeviceKernel(const DeviceKernel&) = delete;
		DeviceKernel(DeviceKernel&&) = delete;
		DeviceKernel& operator=(const DeviceKernel&) = delete;
		DeviceKernel& operator=(DeviceKernel&&) = delete;
	}; // class DeviceKernel
} // namespace monju

#endif // _MONJU_KERNEL_BASE_H__