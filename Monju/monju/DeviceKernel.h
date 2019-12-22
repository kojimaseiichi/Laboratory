#pragma once

#ifndef _MONJU_DEVICE_KERNEL_H__
#define _MONJU_DEVICE_KERNEL_H__

#include <string>
#include <map>
#include <CL/cl.h>
#include "DeviceContext.h"
#include "DeviceProgram.h"
#include "Device.h"
#include "OpenClException.h"
#include "util_str.h"

namespace monju {

	// OpenCLのカーネルラッパー基本クラス
	// カーネルソースのテンプレート変数の反映、コンパイル、実行を管理
	class DeviceKernel
	{
		// 生成・初期化・破棄
	public:
		DeviceKernel()
		{
			_p_device = nullptr;
			_p_program = nullptr;
			_kernel = nullptr;
			_dim = 0;
		}
		virtual ~DeviceKernel();

		// コピー禁止
	protected:
		DeviceKernel(const DeviceKernel&) = delete;
		DeviceKernel& operator=(const DeviceKernel&) = delete;

	protected:
		// 外部依存リソース
		Device*			_p_device;
		DeviceProgram*	_p_program;
		std::string		_kernel_name;
		cl_kernel		_kernel;
		cl_int			_dim;
		std::vector<size_t>	_global_work_size;
		std::vector<size_t> _local_work_size;

	protected:
		// コンパイル済みカーネルプログラムをデバイスに配置して実行可能な状態に遷移
		cl_kernel			_createKernel(cl_program program, std::string kernel_name, std::map<std::string, std::string>& params)
		{
			std::string parameterized_kernel_name = util_str::parameterizePlaceholders(kernel_name, params);
			cl_int error_code;
			cl_kernel kernel = clCreateKernel(program, parameterized_kernel_name.c_str(), &error_code);
			if (error_code != CL_SUCCESS)
				throw OpenClException(error_code);
			return kernel;
		}
		// カーネルを実行可能な状態に遷移（_compileProgram、_createKernel）
		void				_initKernel(cl_device_id device_id, cl_program program, std::string kernel_name, std::map<std::string, std::string>& params)
		{
			_kernel = _createKernel(program, kernel_name, params);
		}
		// プログラムコンパイル(CLファイル)、カーネル生成
		void				_create(
			Device& device,
			DeviceProgram& program,
			std::string kernel_name,
			std::map<std::string, std::string>& params,
			std::vector<size_t>& global_work_size,
			std::vector<size_t>& local_work_size)
		{
			_p_device = &device;
			_p_program = &program;
			_kernel_name = kernel_name;
			_global_work_size = global_work_size;
			_local_work_size = _local_work_size;
			_initKernel(device.getClDeviceId(), program.program(), kernel_name, params);
		}
		// デバイス上に配置済みのカーネルプログラムを実行
		// 引数は事前に設定しておく必要がある
		void				_run()
		{
			cl_int error_code = clEnqueueNDRangeKernel(
				_p_device->getClCommandQueue(),	// OpenCLキュー
				_kernel,						// カーネル
				_dim,							// 2次元
				nullptr,						// global work offset
				_global_work_size.data(),		// global work size
				_local_work_size.data(),		// local work size
				0,								// number of events in wait list
				nullptr,						// event wait list
				nullptr);						// event
			if (error_code != CL_SUCCESS)
				throw OpenClException(error_code);
		}

		// プロパティ
	public:
		Device&				getDevice() const { return *_p_device; }
		DeviceProgram&		getProgram() const { return *_p_program; }
		std::string			getKernelName() const { return _kernel_name; }
		cl_kernel			getKernel() const { return _kernel; }
		std::vector<size_t>	globalWorkSize() const { return _global_work_size; }
		std::vector<size_t>	localWorkSize() const { return _local_work_size; }

		// 公開関数
	public:
		// OpenCLカーネル生成
		void		create(
			Device& device,
			DeviceProgram& program,
			std::string kernel_name,
			std::map<std::string, std::string>& params,
			std::vector<size_t>& global_work_size,
			std::vector<size_t>& local_work_size
		)
		{
			if (global_work_size.size() != local_work_size.size())
				throw MonjuException("global_work_sizeとlocal_work_sizeの次元が一致しない");
			_create(device, program, kernel_name, params, global_work_size, local_work_size);
		}
		void		release()
		{
			if (_kernel != nullptr)
			{
				cl_int error_code = clReleaseKernel(_kernel);
				if (error_code != CL_SUCCESS)
					throw OpenClException(error_code);
				_kernel = nullptr;
			}
			_p_device = nullptr;
			_p_program = nullptr;
			_kernel_name = "";
			_dim = 0;
			_global_work_size.clear();
			_local_work_size.clear();
		}
		void		compute(
			std::vector<size_t>& global_work_size
		);
		void		compute(
			std::vector<size_t>& global_work_size,
			std::vector<size_t>& local_work_size
		)
		{
			_run();
		}

	}; // class DeviceKernel
} // namespace monju

#endif // _MONJU_KERNEL_BASE_H__