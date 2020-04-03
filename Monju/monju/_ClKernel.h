#pragma once
#ifndef _MONJU__CL_KERNEL_H__
#define _MONJU__CL_KERNEL_H__

#include "MonjuTypes.h"

namespace monju
{
	class _ClKernel
	{
	private:
		cl_program _program;
		std::string _kernelName;
		param_map _params;
		cl_kernel _kernel;	// ����\��

		cl_kernel _createKernel(cl_program program, std::string& kernelName, param_map& params);
		void _releaseKernel();

	public:
		_ClKernel(cl_program program, std::string kernelName, param_map params);
		~_ClKernel();
		cl_kernel clKernel() const;

		// �R�s�[�֎~�E���[�u�֎~
	public:
		_ClKernel(const _ClKernel&) = delete;
		_ClKernel(_ClKernel&&) = delete;
		_ClKernel& operator=(const _ClKernel&) = delete;
		_ClKernel& operator=(_ClKernel&&) = delete;
	};

}

#endif // !_MONJU__CL_KERNEL_H__

