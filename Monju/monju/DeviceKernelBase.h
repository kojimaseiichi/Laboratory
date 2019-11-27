#pragma once
#ifndef _MONJU_DEVICE_KERNEL_BASE_H__
#define _MONJU_DEVICE_KERNEL_BASE_H__

#include <string>
#include <map>
#include <CL/cl.h>
#include "GpuDeviceContext.h"
#include "Device.h"

namespace monju {

	// OpenCLのカーネルラッパー基本クラス
	// カーネルソースのテンプレート変数の反映、コンパイル、実行を管理
	class DeviceKernelBase
	{
		// 型
	protected:
		using params_map = std::map<std::string, std::string>;

		// 生成・初期化・破棄
	public:
		DeviceKernelBase();
		virtual ~DeviceKernelBase();

		// コピー禁止
	protected:
		DeviceKernelBase(const DeviceKernelBase&) = delete;
		DeviceKernelBase& operator=(const DeviceKernelBase&) = delete;

	protected:
		// 外部依存リソース
		GpuDeviceContext* _p_dc;
		Device* _p_device;
		std::string			_source_path;	// CLファイル名
		std::string			_kernel_name;
		std::vector<size_t> _global_work_size;
		std::vector<size_t> _local_work_size;
		// 責任範囲のリソース
		cl_program			_program;
		cl_kernel			_kernel;

	protected:
		// カーネルコンパイルしてカーネルプログラムを生成（この状態では実行できない）
		// OpenCLカーネルソースを読み込み、テンプレート変数の具体化、カーネルコンパイル
		cl_program			_compileProgram(cl_context context, cl_device_id device_id, std::string file_path, params_map params);
		// コンパイル済みカーネルプログラムをデバイスに配置して実行可能な状態に遷移
		cl_kernel			_createKernel(cl_program program, std::string kernel_name);
		// カーネルを実行可能な状態に遷移（_compileProgram、_createKernel）
		void				_initKernel(cl_context context, cl_device_id device_id, std::string source_path, std::string kernel_name, params_map params);
		// カーネルソースの内容を文字列で取得
		std::string			_getSource(std::string souce_path);
		// カーネルソース内のテンプレート変数を具体化
		std::string			_editSource(std::string source, std::map<std::string, std::string> params);
		// プログラムコンパイル(CLファイル)、カーネル生成
		void				_create(GpuDeviceContext& context, Device& device, std::string source_path, std::string kernel_name);
		// デバイス上に配置済みのカーネルプログラムを実行
		// 引数は事前に設定しておく必要がある
		cl_int				_run(cl_int dim, const size_t* global_work_size, const size_t* local_work_size);
		// 
		void				_setWorkItem(std::vector<size_t>& global_work_size);
		void				_setWorkItem(std::vector<size_t>& global_work_size, std::vector<size_t>& local_work_size);

		// プロパティ
	public:
		GpuDeviceContext*	getDeviceContext() const { return _p_dc; }
		Device*				getDevice() const { return _p_device; }
		std::string			getSourceName() const { return _source_path; }
		std::string			getKernelName() const { return _kernel_name; }
		cl_program			getProgram() const { return _program; }
		cl_kernel			getKernel() const { return _kernel; }
		std::vector<size_t>	getGlobalWorkSize() const { return _global_work_size; }
		std::vector<size_t>	getLocalWorkSize() const { return _local_work_size; }

		// 公開関数
	public:
		virtual void		create(
			GpuDeviceContext& context,
			Device& device,
			std::string source_path,
			std::string kernel_name,
			std::vector<size_t>& global_work_size,
			std::vector<size_t>& local_work_size
		)
		{
			_create(context, device, source_path, kernel_name);
			_setWorkItem(global_work_size, local_work_size);
		}
		virtual void		create(
			GpuDeviceContext& context,
			Device& device,
			std::string source_path,
			std::string kernel_name,
			std::vector<size_t>& global_work_size
		)
		{
			_create(context, device, source_path, kernel_name);
			_setWorkItem(global_work_size);
		}
		virtual void		release();
		virtual void		compute()
		{
			_run(
				_global_work_size.size(),
				_global_work_size.data(),
				_local_work_size.size() == 0 ? nullptr : _local_work_size.data()
			);
		}

	}; // class DeviceKernelBase
} // namespace monju

#endif // _MONJU_KERNEL_BASE_H__