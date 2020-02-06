#include "OpenClKernelArgs.h"

monju::OpenClKernelArgs::OpenClKernelArgs(std::weak_ptr<OpenClKernel> kernel)
{
	_kernel = kernel;
}

monju::OpenClKernelArgs::~OpenClKernelArgs()
{
}

void monju::OpenClKernelArgs::push(size_t size, const void* p)
{
	Arg arg;
	arg.size = size;
	arg.p = p;
	_args.push_back(arg);
}

void monju::OpenClKernelArgs::flush()
{
	{
		auto pk = _kernel.lock();
		for (int n = 0; n < _args.size(); n++)
		{
			const auto& r = _args.at(n);
			clSetKernelArg(pk->clKernel, n, r.size, r.p);
		}
	}
	_args.clear();
}

size_t monju::OpenClKernelArgs::count() const
{
	return _args.size();
}
