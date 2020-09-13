#pragma once

#ifndef _MONJU_DEVICE_KERNEL_H__
#define _MONJU_DEVICE_KERNEL_H__

#include "MonjuTypes.h"

namespace monju {

	// OpenCLのカーネルラッパー基本クラス
	// カーネルソースのテンプレート変数の反映、コンパイル、実行を管理
	class DeviceKernel
	{
		// 生成・初期化・破棄
	public:
		DeviceKernel();
		virtual ~DeviceKernel();

	protected:
		// 外部依存リソース
		DeviceProgram*	_p_program;
		std::string		_kernel_name;
		cl_kernel		_kernel;

	protected:
		// コンパイル済みカーネルプログラムをデバイスに配置して実行可能な状態に遷移
		cl_kernel			_create_kernel(cl_program program, std::string kernel_name, param_map& params);
		// カーネルを実行可能な状態に遷移（_compileProgram、_createKernel）
		void				_initKernel(cl_program program, std::string kernel_name, param_map& params);
		// プログラムコンパイル(CLファイル)、カーネル生成
		void				_create(
			DeviceProgram& program,
			std::string kernel_name,
			param_map& params);
		// デバイス上に配置済みのカーネルプログラムを実行
		// 引数は事前に設定しておく必要がある
		void				_run(
			Device& device,
			std::vector<size_t>& global_work_size,
			std::vector<size_t>& local_work_size);

		void				_run(
			Device& device,
			std::vector<size_t>& global_work_size);

		// プロパティ
	public:
		DeviceProgram&		getProgram() const { return *_p_program; }
		std::string			getKernelName() const { return _kernel_name; }
		cl_kernel			getKernel() const { return _kernel; }

		// 公開関数
	public:
		// OpenCLカーネル生成
		void		create(
			DeviceProgram& program,
			std::string kernel_name,
			param_map& params
		);
		// OpenCLカーネル解放
		void		release();
		// カーネル計算実行
		void		compute(
			Device& device,
			std::vector<size_t>& global_work_size
		);
		// カーネル計算実行
		void		compute(
			Device& device,
			std::vector<size_t>& global_work_size,
			std::vector<size_t>& local_work_size
		);

		// コピー禁止・ムーブ禁止
	protected:
		DeviceKernel(const DeviceKernel&) = delete;
		DeviceKernel(DeviceKernel&&) = delete;
		DeviceKernel& operator=(const DeviceKernel&) = delete;
		DeviceKernel& operator=(DeviceKernel&&) = delete;
	}; // class DeviceKernel
} // namespace monju

#endif // _MONJU_KERNEL_BASE_H__