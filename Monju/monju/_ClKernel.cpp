#include "_ClKernel.h"

cl_kernel monju::_ClKernel::_createKernel(cl_program program, std::string& kernelName, params_map& params)
{
	std::string parameterized_kernel_name = util_str::parameterizePlaceholders(kernelName, params);
	cl_int error;
	cl_kernel kernel = clCreateKernel(program, parameterized_kernel_name.c_str(), &error);
	if (error != CL_SUCCESS)
		throw OpenClException(error);
	return kernel;
}

void monju::_ClKernel::_releaseKernel()
{
	if (_kernel == nullptr)
		return;
	cl_int error = clReleaseKernel(_kernel);
	if (error != CL_SUCCESS)
		throw OpenClException(error, "clReleaseKernel");
	_kernel = nullptr;
}

monju::_ClKernel::_ClKernel(cl_program program, std::string kernelName, params_map params)
{
	_kernel = _createKernel(program, kernelName, params);
	_program = program;
	_kernelName = kernelName;
	_params = params;
}

monju::_ClKernel::~_ClKernel()
{
	_releaseKernel();
}

cl_kernel monju::_ClKernel::clKernel() const
{
	return _kernel;
}
