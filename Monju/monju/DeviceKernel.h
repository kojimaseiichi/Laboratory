#pragma once

#ifndef _MONJU_DEVICE_KERNEL_H__
#define _MONJU_DEVICE_KERNEL_H__

#include <string>
#include <map>
#include <CL/cl.h>
#include "DeviceContext.h"
#include "Device.h"
#include "OpenClException.h"

namespace monju {

	// OpenCLのカーネルラッパー基本クラス
	// カーネルソースのテンプレート変数の反映、コンパイル、実行を管理
	class DeviceKernel
	{
		// 型
	public:
using params_map = std::map<std::string, std::string>;

		// 生成・初期化・破棄
	public:
		DeviceKernel();
		virtual ~DeviceKernel();

		// コピー禁止
	protected:
		DeviceKernel(const DeviceKernel&) = delete;
		DeviceKernel& operator=(const DeviceKernel&) = delete;

	protected:
		// 外部依存リソース
		Device* _p_device;
std::string			_source_path;	// CLファイル名
std::string			_kernel_name;

cl_program			_program;
		cl_kernel			_kernel;

	protected:
		// カーネルコンパイルしてカーネルプログラムを生成（この状態では実行できない）
		// OpenCLカーネルソースを読み込み、テンプレート変数の具体化、カーネルコンパイル
		cl_program			_compileProgram(cl_context context, cl_device_id device_id, std::string file_path, params_map& params);
		// コンパイル済みカーネルプログラムをデバイスに配置して実行可能な状態に遷移
		cl_kernel			_createKernel(cl_program program, std::string kernel_name, params_map& params);
		// カーネルを実行可能な状態に遷移（_compileProgram、_createKernel）
		void				_initKernel(cl_context context, cl_device_id device_id, std::string source_path, std::string kernel_name, params_map& params);
		// カーネルソースの内容を文字列で取得
		std::string			_getSource(std::string souce_path);
		// カーネルソース内のテンプレート変数を具体化
		std::string			_parameterize(std::string source, params_map& params);
		// プログラムコンパイル(CLファイル)、カーネル生成
		void				_create(Device& device, std::string source_path, std::string kernel_name, params_map& params);
		// デバイス上に配置済みのカーネルプログラムを実行
		// 引数は事前に設定しておく必要がある
		void				_run(cl_int dim, const size_t* global_work_size, const size_t* local_work_size);

		// プロパティ
	public:
		Device*				getDevice() const { return _p_device; }
		std::string			getSourceName() const { return _source_path; }
		std::string			getKernelName() const { return _kernel_name; }
		cl_program			getProgram() const { return _program; }
		cl_kernel			getKernel() const { return _kernel; }
		int					getNumArgs()
		{

		}

		// 公開関数
	public:
		// OpenCLカーネル生成
		void		create(
			Device& device,
			std::string source_path,
			std::string kernel_name,
			params_map& params
		);
		void		release();
		void		compute(
			std::vector<size_t>& global_work_size
		);
		void		compute(
			std::vector<size_t>& global_work_size,
			std::vector<size_t>& local_work_size
		);

	}; // class DeviceKernel
} // namespace monju

#endif // _MONJU_KERNEL_BASE_H__