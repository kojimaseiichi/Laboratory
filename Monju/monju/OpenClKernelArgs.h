#pragma once
#ifndef _MONJU_OPEN_CL_KERNEL_ARGS_H__
#define _MONJU_OPEN_CL_KERNEL_ARGS_H__


#include "OpenClKernel.h"

namespace monju
{
	class OpenClKernelArgs
	{
	private:
		struct Arg
		{
			size_t size;
			const void* p;
		};
	private:
		std::weak_ptr<OpenClKernel> _kernel;
		std::vector<Arg> _args;

	public:
		OpenClKernelArgs(std::weak_ptr<OpenClKernel> kernel);
		~OpenClKernelArgs();
		void push(size_t size, const void* p);
		void flush();
		size_t count() const;

		// コピー禁止・ムーブ禁止
	public:
		OpenClKernelArgs(const OpenClKernelArgs&) = delete;
		OpenClKernelArgs(OpenClKernelArgs&&) = delete;
		OpenClKernelArgs& operator=(const OpenClKernelArgs&) = delete;
		OpenClKernelArgs& operator=(OpenClKernelArgs&&) = delete;
	};
}

#endif // !_MONJU_OPEN_CL_KERNEL_ARGS_H__
