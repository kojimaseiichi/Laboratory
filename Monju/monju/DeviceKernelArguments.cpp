#include "DeviceKernelArguments.h"

monju::DeviceKernelArguments::DeviceKernelArguments(DeviceMemory& memory)
{
	_p_memory = &memory;
}

monju::DeviceKernelArguments::~DeviceKernelArguments()
{

}

void monju::DeviceKernelArguments::_clSetKernelArg(cl_kernel kernel, cl_uint arg_index, size_t arg_size, const void* arg_value)
{
	cl_int error_code = clSetKernelArg(kernel, arg_index, arg_size, arg_value);
	if (error_code != CL_SUCCESS)
		throw OpenClException(error_code);
}

void monju::DeviceKernelArguments::push(VariableKind kind, bool output)
{
	cl_mem m = _p_memory->getMemory(kind);
	_Argument arg;
	arg.arg_size = sizeof(cl_mem);
	arg.arg_value.__cl_mem = m;
	_arguments.push_back(arg);
	if (output)
		_outputParams.insert(kind);
}

void monju::DeviceKernelArguments::push(cl_float value)
{
	_Argument arg;
	arg.arg_size = sizeof(cl_float);
	arg.arg_value.__cl_float = value;
	_arguments.push_back(arg);
}

void monju::DeviceKernelArguments::push(cl_int value)
{
	_Argument arg;
	arg.arg_size = sizeof(cl_int);
	arg.arg_value.__cl_int = value;
	_arguments.push_back(arg);
}

void monju::DeviceKernelArguments::stackArguments(DeviceKernel& kernel)
{
	for (size_t idx = 0; idx < _arguments.size(); idx++)
	{
		auto const& elem = _arguments[idx];
		_clSetKernelArg(kernel.getKernel(), (cl_uint)idx, elem.arg_size, &(elem.arg_value));
	}
}

void monju::DeviceKernelArguments::reset()
{
	_arguments.clear();
	_outputParams.clear();
}
