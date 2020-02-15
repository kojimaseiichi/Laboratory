#pragma once
#ifndef _MONJU__CL_KERNEL_H__
#define _MONJU__CL_KERNEL_H__

#include "_ClProgram.h"

namespace monju
{
	class _ClKernel
	{
	public:
		using params_map = std::map<std::string, std::string>;

	private:
		cl_program _program;
		std::string _kernelName;
		params_map _params;
		cl_kernel _kernel;	// 解放予定

		cl_kernel _createKernel(cl_program program, std::string& kernelName, params_map& params);
		void _releaseKernel();

	public:
		_ClKernel(cl_program program, std::string kernelName, params_map params);
		~_ClKernel();
		cl_kernel clKernel() const;

		// コピー禁止・ムーブ禁止
	public:
		_ClKernel(const _ClKernel&) = delete;
		_ClKernel(_ClKernel&&) = delete;
		_ClKernel& operator=(const _ClKernel&) = delete;
		_ClKernel& operator=(_ClKernel&&) = delete;
	};

}

#endif // !_MONJU__CL_KERNEL_H__

