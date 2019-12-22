#pragma once

#ifndef _MONJU_DEVICE_KERNEL_H__
#define _MONJU_DEVICE_KERNEL_H__

#include <string>
#include <map>
#include <CL/cl.h>
#include "DeviceContext.h"
#include "Device.h"
#include "OpenClException.h"

namespace monju {

	// OpenCL�̃J�[�l�����b�p�[��{�N���X
	// �J�[�l���\�[�X�̃e���v���[�g�ϐ��̔��f�A�R���p�C���A���s���Ǘ�
	class DeviceKernel
	{
		// �^
	public:
using params_map = std::map<std::string, std::string>;

		// �����E�������E�j��
	public:
		DeviceKernel();
		virtual ~DeviceKernel();

		// �R�s�[�֎~
	protected:
		DeviceKernel(const DeviceKernel&) = delete;
		DeviceKernel& operator=(const DeviceKernel&) = delete;

	protected:
		// �O���ˑ����\�[�X
		Device* _p_device;
std::string			_source_path;	// CL�t�@�C����
std::string			_kernel_name;

cl_program			_program;
		cl_kernel			_kernel;

	protected:
		// �J�[�l���R���p�C�����ăJ�[�l���v���O�����𐶐��i���̏�Ԃł͎��s�ł��Ȃ��j
		// OpenCL�J�[�l���\�[�X��ǂݍ��݁A�e���v���[�g�ϐ��̋�̉��A�J�[�l���R���p�C��
		cl_program			_compileProgram(cl_context context, cl_device_id device_id, std::string file_path, params_map& params);
		// �R���p�C���ς݃J�[�l���v���O�������f�o�C�X�ɔz�u���Ď��s�\�ȏ�ԂɑJ��
		cl_kernel			_createKernel(cl_program program, std::string kernel_name, params_map& params);
		// �J�[�l�������s�\�ȏ�ԂɑJ�ځi_compileProgram�A_createKernel�j
		void				_initKernel(cl_context context, cl_device_id device_id, std::string source_path, std::string kernel_name, params_map& params);
		// �J�[�l���\�[�X�̓��e�𕶎���Ŏ擾
		std::string			_getSource(std::string souce_path);
		// �J�[�l���\�[�X���̃e���v���[�g�ϐ�����̉�
		std::string			_parameterize(std::string source, params_map& params);
		// �v���O�����R���p�C��(CL�t�@�C��)�A�J�[�l������
		void				_create(Device& device, std::string source_path, std::string kernel_name, params_map& params);
		// �f�o�C�X��ɔz�u�ς݂̃J�[�l���v���O���������s
		// �����͎��O�ɐݒ肵�Ă����K�v������
		void				_run(cl_int dim, const size_t* global_work_size, const size_t* local_work_size);

		// �v���p�e�B
	public:
		Device*				getDevice() const { return _p_device; }
		std::string			getSourceName() const { return _source_path; }
		std::string			getKernelName() const { return _kernel_name; }
		cl_program			getProgram() const { return _program; }
		cl_kernel			getKernel() const { return _kernel; }
		int					getNumArgs()
		{

		}

		// ���J�֐�
	public:
		// OpenCL�J�[�l������
		void		create(
			Device& device,
			std::string source_path,
			std::string kernel_name,
			params_map& params
		);
		void		release();
		void		compute(
			std::vector<size_t>& global_work_size
		);
		void		compute(
			std::vector<size_t>& global_work_size,
			std::vector<size_t>& local_work_size
		);

	}; // class DeviceKernel
} // namespace monju

#endif // _MONJU_KERNEL_BASE_H__