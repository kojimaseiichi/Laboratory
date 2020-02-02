#include "DeviceKernel.h"
#include "MonjuException.h" 
#include <fstream>
#include <map>
#include <boost/regex.hpp>

monju::DeviceKernel::~DeviceKernel()
{
	release();
}

// コンパイル済みカーネルプログラムをデバイスに配置して実行可能な状態に遷移

cl_kernel monju::DeviceKernel::_createKernel(cl_program program, std::string kernel_name, std::map<std::string, std::string>& params)
{
	std::string parameterized_kernel_name = util_str::parameterizePlaceholders(kernel_name, params);
	cl_int error_code;
	cl_kernel kernel = clCreateKernel(program, parameterized_kernel_name.c_str(), &error_code);
	if (error_code != CL_SUCCESS)
		throw OpenClException(error_code);
	return kernel;
}

// カーネルを実行可能な状態に遷移（_compileProgram、_createKernel）

void monju::DeviceKernel::_initKernel(cl_program program, std::string kernel_name, std::map<std::string, std::string>& params)
{
	_kernel = _createKernel(program, kernel_name, params);
}

// プログラムコンパイル(CLファイル)、カーネル生成

void monju::DeviceKernel::_create(DeviceProgram& program, std::string kernel_name, std::map<std::string, std::string>& params)
{
	_p_program = &program;
	_kernel_name = kernel_name;
	_initKernel(program.program(), kernel_name, params);
}

// デバイス上に配置済みのカーネルプログラムを実行
// 引数は事前に設定しておく必要がある

void monju::DeviceKernel::_run(Device& device, std::vector<size_t>& global_work_size, std::vector<size_t>& local_work_size)
{
	if (global_work_size.size() != local_work_size.size())
		throw MonjuException("global_work_sizeとlocal_work_sizeの次元が一致しない");
	cl_uint dim = static_cast<cl_uint>(global_work_size.size());
	cl_event eventWait;
	cl_int error_code = clEnqueueNDRangeKernel(
		device.getClCommandQueue(),	// OpenCLキュー
		_kernel,						// カーネル
		dim,							// 2次元
		nullptr,						// global work offset
		global_work_size.data(),		// global work size
		local_work_size.data(),			// local work size
		0,								// number of events in wait list
		nullptr,						// event wait list
		&eventWait);					// event
	if (error_code != CL_SUCCESS)
		throw OpenClException(error_code);
	error_code = clWaitForEvents(1, &eventWait);
	if (error_code != CL_SUCCESS)
		throw OpenClException(error_code);
	error_code = clReleaseEvent(eventWait);
	if (error_code != CL_SUCCESS)
		throw OpenClException(error_code);
}

void monju::DeviceKernel::_run(Device& device, std::vector<size_t>& global_work_size)
{
	cl_uint dim = static_cast<cl_uint>(global_work_size.size());
	cl_event eventWait;
	cl_int error_code = clEnqueueNDRangeKernel(
		device.getClCommandQueue(),	// OpenCLキュー
		_kernel,						// カーネル
		dim,							// 2次元
		nullptr,						// global work offset
		global_work_size.data(),		// global work size
		nullptr,						// local work size(null)
		0,								// number of events in wait list
		nullptr,						// event wait list
		&eventWait);						// event
	if (error_code != CL_SUCCESS)
		throw OpenClException(error_code);
	error_code = clWaitForEvents(1, &eventWait);
	if (error_code != CL_SUCCESS)
		throw OpenClException(error_code);
	error_code = clReleaseEvent(eventWait);
	if (error_code != CL_SUCCESS)
		throw OpenClException(error_code);
}

// OpenCLカーネル生成

void monju::DeviceKernel::create(DeviceProgram& program, std::string kernel_name, std::map<std::string, std::string>& params)
{
	_create(program, kernel_name, params);
}

// OpenCLカーネル解放

void monju::DeviceKernel::release()
{
	if (_kernel != nullptr)
	{
		cl_int error_code = clReleaseKernel(_kernel);
		if (error_code != CL_SUCCESS)
			throw OpenClException(error_code);
		_kernel = nullptr;
	}
	_p_program = nullptr;
	_kernel_name = "";
}

// カーネル計算実行

void monju::DeviceKernel::compute(Device& device, std::vector<size_t>& global_work_size)
{
	_run(device, global_work_size);
}

// カーネル計算実行

void monju::DeviceKernel::compute(Device& device, std::vector<size_t>& global_work_size, std::vector<size_t>& local_work_size)
{
	_run(device, global_work_size, local_work_size);
}

// プログラムコンパイル(CLファイル)、カーネル生成

