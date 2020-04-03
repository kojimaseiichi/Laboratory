#pragma once
#ifndef _MONJU_CL_KERNEL_H__
#define _MONJU_CL_KERNEL_H__

#include "MonjuTypes.h"

namespace monju
{
	class ClKernel
	{
	private:
		std::shared_ptr<ClMachine> _clMachine;
		std::shared_ptr<_ClContext> _clContext;	// ClMachine����擾
		std::string _sourcePath;
		std::string _kernelName;
		param_map _params;

		std::shared_ptr<_ClProgram> _clProgram;	// ����\��
		std::shared_ptr<_ClKernel> _clKernel;	// ����\��

		void _createProgram();
		void _createKernel();
		void _releaseProgram();
		void _releaseKernel();

	public:
		ClKernel(
			std::weak_ptr<ClMachine> clMachine,
			std::string sourcePath,
			std::string kernelName,
			param_map params);
		ClKernel(
			std::weak_ptr<ClMachine> clMachine,
			std::filesystem::path sourcePath,
			std::string kernelName,
			param_map params);
		~ClKernel();
		cl_program clProgram() const;
		cl_kernel clKernel() const;

		// �R�s�[�֎~�E���[�u�֎~
	public:
		ClKernel(const ClKernel&) = delete;
		ClKernel(ClKernel&&) = delete;
		ClKernel& operator=(const ClKernel&) = delete;
		ClKernel& operator=(ClKernel&&) = delete;
	};

}

#endif // !_MONJU_CL_KERNEL_H__
