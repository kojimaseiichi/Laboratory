#pragma once
#ifndef _MONJU_DEVICE_KERNEL_ARGUMENTS_H__
#define _MONJU_DEVICE_KERNEL_ARGUMENTS_H__

#include "DeviceKernel.h"
#include "DeviceMemory.h"

namespace monju {

	// 
	class DeviceKernelArguments
	{
	private:

		int _count;
		DeviceKernel* _p_kernel;
		DeviceMemory* _p_memory;

	public:
		DeviceKernelArguments(DeviceKernel& kernel, DeviceMemory& memory)
		{
			_count = 0;
			_p_kernel = &kernel;
			_p_memory = &memory;
		}
		~DeviceKernelArguments()
		{

		}

		// ƒRƒs[‹ÖŽ~
	public:
		DeviceKernelArguments(const DeviceKernelArguments&) = delete;
		DeviceKernelArguments& operator =(const DeviceKernelArguments&) = delete;

	private:
		void _clSetKernelArg(cl_kernel kernel, cl_uint arg_index, size_t arg_size, const void* arg_value)
		{
			cl_int error_code = clSetKernelArg(kernel, arg_index, arg_size, arg_value);
			if (error_code != CL_SUCCESS)
				throw OpenClException(error_code);
		}

	public:

		void push(VariableKind kind)
		{
			cl_mem m = _p_memory->getMemory(kind);
			_clSetKernelArg(_p_kernel->getKernel(), _count++, sizeof(cl_mem), &m);
		}

		void push(cl_float value)
		{
			_clSetKernelArg(_p_kernel->getKernel(), _count++, sizeof(cl_float), &value);
		}

		void push(cl_int value)
		{
			_clSetKernelArg(_p_kernel->getKernel(), _count++, sizeof(cl_int), &value);
		}

	};

}

#endif // !_MONJU_DEVICE_KERNEL_ARGUMENTS_H__
