#pragma once
#ifndef _MONJU_DEVICE_KERNEL_ARGUMENTS_H__
#define _MONJU_DEVICE_KERNEL_ARGUMENTS_H__

#include "MonjuTypes.h"

namespace monju {

	// 
	class DeviceKernelArguments
	{
	private:

		// �������
		struct _Argument
		{
			size_t arg_size;
			union {
				cl_mem __cl_mem;
				cl_float __cl_float;
				cl_int __cl_int;
			} arg_value;
		};

		std::vector<_Argument> _arguments;
		std::set<VariableKind> _outputParams;

	public:
		DeviceKernelArguments();
		~DeviceKernelArguments();

		// �R�s�[�֎~
	public:
		DeviceKernelArguments(const DeviceKernelArguments&) = delete;
		DeviceKernelArguments(DeviceKernelArguments&&) = delete;
		DeviceKernelArguments& operator =(const DeviceKernelArguments&) = delete;
		DeviceKernelArguments& operator =(DeviceKernelArguments&&) = delete;

	private:
		void _clSetKernelArg(cl_kernel kernel, cl_uint arg_index, size_t arg_size, const void* arg_value);

		// �v���p�e�B
	public:
		std::set<VariableKind> outputParams() const { return _outputParams; }

	public:
		void push(DeviceMemory& mem, VariableKind kind, bool output);
		void push(cl_float value);
		void push(cl_int value);
		void stackArguments(DeviceKernel& kernel);
		void reset();
	};

}

#endif // !_MONJU_DEVICE_KERNEL_ARGUMENTS_H__
