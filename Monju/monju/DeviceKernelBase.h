#pragma once
#ifndef _MONJU_DEVICE_KERNEL_BASE_H__
#define _MONJU_DEVICE_KERNEL_BASE_H__

#include <string>
#include <map>
#include <CL/cl.h>
#include "GpuDeviceContext.h"
#include "Device.h"

namespace monju {

	// OpenCL�̃J�[�l�����b�p�[��{�N���X
	// �J�[�l���\�[�X�̃e���v���[�g�ϐ��̔��f�A�R���p�C���A���s���Ǘ�
	class DeviceKernelBase
	{
		// �^
	protected:
		using params_map = std::map<std::string, std::string>;

		// �����E�������E�j��
	public:
		DeviceKernelBase();
		virtual ~DeviceKernelBase();

		// �R�s�[�֎~
	protected:
		DeviceKernelBase(const DeviceKernelBase&) = delete;
		DeviceKernelBase& operator=(const DeviceKernelBase&) = delete;

	protected:
		// �O���ˑ����\�[�X
		GpuDeviceContext* _p_dc;
		Device* _p_device;
		std::string			_source_path;	// CL�t�@�C����
		std::string			_kernel_name;
		std::vector<size_t> _global_work_size;
		std::vector<size_t> _local_work_size;
		// �ӔC�͈͂̃��\�[�X
		cl_program			_program;
		cl_kernel			_kernel;

	protected:
		// �J�[�l���R���p�C�����ăJ�[�l���v���O�����𐶐��i���̏�Ԃł͎��s�ł��Ȃ��j
		// OpenCL�J�[�l���\�[�X��ǂݍ��݁A�e���v���[�g�ϐ��̋�̉��A�J�[�l���R���p�C��
		cl_program			_compileProgram(cl_context context, cl_device_id device_id, std::string file_path, params_map params);
		// �R���p�C���ς݃J�[�l���v���O�������f�o�C�X�ɔz�u���Ď��s�\�ȏ�ԂɑJ��
		cl_kernel			_createKernel(cl_program program, std::string kernel_name);
		// �J�[�l�������s�\�ȏ�ԂɑJ�ځi_compileProgram�A_createKernel�j
		void				_initKernel(cl_context context, cl_device_id device_id, std::string source_path, std::string kernel_name, params_map params);
		// �J�[�l���\�[�X�̓��e�𕶎���Ŏ擾
		std::string			_getSource(std::string souce_path);
		// �J�[�l���\�[�X���̃e���v���[�g�ϐ�����̉�
		std::string			_editSource(std::string source, std::map<std::string, std::string> params);
		// �v���O�����R���p�C��(CL�t�@�C��)�A�J�[�l������
		void				_create(GpuDeviceContext& context, Device& device, std::string source_path, std::string kernel_name);
		// �f�o�C�X��ɔz�u�ς݂̃J�[�l���v���O���������s
		// �����͎��O�ɐݒ肵�Ă����K�v������
		cl_int				_run(cl_int dim, const size_t* global_work_size, const size_t* local_work_size);
		// 
		void				_setWorkItem(std::vector<size_t>& global_work_size);
		void				_setWorkItem(std::vector<size_t>& global_work_size, std::vector<size_t>& local_work_size);

		// �v���p�e�B
	public:
		GpuDeviceContext*	getDeviceContext() const { return _p_dc; }
		Device*				getDevice() const { return _p_device; }
		std::string			getSourceName() const { return _source_path; }
		std::string			getKernelName() const { return _kernel_name; }
		cl_program			getProgram() const { return _program; }
		cl_kernel			getKernel() const { return _kernel; }
		std::vector<size_t>	getGlobalWorkSize() const { return _global_work_size; }
		std::vector<size_t>	getLocalWorkSize() const { return _local_work_size; }

		// ���J�֐�
	public:
		virtual void		create(
			GpuDeviceContext& context,
			Device& device,
			std::string source_path,
			std::string kernel_name,
			std::vector<size_t>& global_work_size,
			std::vector<size_t>& local_work_size
		)
		{
			_create(context, device, source_path, kernel_name);
			_setWorkItem(global_work_size, local_work_size);
		}
		virtual void		create(
			GpuDeviceContext& context,
			Device& device,
			std::string source_path,
			std::string kernel_name,
			std::vector<size_t>& global_work_size
		)
		{
			_create(context, device, source_path, kernel_name);
			_setWorkItem(global_work_size);
		}
		virtual void		release();
		virtual void		compute()
		{
			_run(
				_global_work_size.size(),
				_global_work_size.data(),
				_local_work_size.size() == 0 ? nullptr : _local_work_size.data()
			);
		}

	}; // class DeviceKernelBase
} // namespace monju

#endif // _MONJU_KERNEL_BASE_H__