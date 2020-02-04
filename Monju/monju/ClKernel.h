#pragma once
#ifndef _MONJU_CL_KERNEL_H__
#define _MONJU_CL_KERNEL_H__

#include "ClProgram.h"

namespace monju
{
	class ClKernel
	{
	public:
		using params_map = std::map<std::string, std::string>;

	private:
		cl_program _program;
		std::string _kernelName;
		params_map _params;
		cl_kernel _kernel;	// 解放予定

		cl_kernel _createKernel(cl_program program, std::string& kernelName, std::map<std::string, std::string>& params)
		{
			std::string parameterized_kernel_name = util_str::parameterizePlaceholders(kernelName, params);
			cl_int error;
			cl_kernel kernel = clCreateKernel(program, parameterized_kernel_name.c_str(), &error);
			if (error != CL_SUCCESS)
				throw OpenClException(error);
			return kernel;
		}
		void _run(cl_command_queue commandQueue, std::vector<size_t>& globalWorkSize, std::vector<size_t>& localWorkSize)
		{
			if (globalWorkSize.size() != localWorkSize.size())
				throw MonjuException("global_work_sizeとlocal_work_sizeの次元が一致しない");
			cl_uint dim = static_cast<cl_uint>(globalWorkSize.size());
			cl_event eventWait;
			cl_int error = clEnqueueNDRangeKernel(
				commandQueue,					// OpenCLキュー
				_kernel,						// カーネル
				dim,							// 2次元
				nullptr,						// global work offset
				globalWorkSize.data(),			// global work size
				localWorkSize.data(),			// local work size
				0,								// number of events in wait list
				nullptr,						// event wait list
				&eventWait);					// event
			if (error != CL_SUCCESS)
				throw OpenClException(error);
		}
		void _releaseKernel()
		{
			if (_kernel == nullptr)
				return;
			cl_int error = clReleaseKernel(_kernel);
			if (error != CL_SUCCESS)
				throw OpenClException(error, "clReleaseKernel");
			_kernel = nullptr;
		}

	public:
		ClKernel(cl_program program, std::string kernelName, params_map params)
		{
			_kernel = _createKernel(program, kernelName, params);
			_program = program;
			_kernelName = kernelName;
			_params = params;
		}
		~ClKernel()
		{
			_releaseKernel();
		}
		void run(cl_command_queue commandQueue, std::vector<size_t> globalWorkSize, std::vector<size_t>& localWorkSize)
		{
			_run(commandQueue, globalWorkSize, localWorkSize);
		}

		// コピー禁止・ムーブ禁止
	public:
		ClKernel(const ClKernel&) = delete;
		ClKernel(ClKernel&&) = delete;
		ClKernel& operator=(const ClKernel&) = delete;
		ClKernel& operator=(ClKernel&&) = delete;
	};

}

#endif // !_MONJU_CL_KERNEL_H__

