#pragma once
#ifndef _MONJU_CL_KERNEL_H__
#define _MONJU_CL_KERNEL_H__

#include <memory>
#include <string>
#include <map>
#include <filesystem>
#include <CL/cl.h>

namespace monju
{
	/* 前方宣言 */
	class ClMachine;
	class _ClContext;
	class _ClProgram;
	class _ClKernel;

	using param_map = std::map<std::string, std::string>;

	class ClKernel
	{
	private:
		std::shared_ptr<ClMachine> _clMachine;
		std::shared_ptr<_ClContext> _clContext;	// ClMachineから取得
		std::string _sourcePath;
		std::string _kernelName;
		param_map _params;

		std::shared_ptr<_ClProgram> _clProgram;	// 解放予定
		std::shared_ptr<_ClKernel> _clKernel;	// 解放予定

	private:
		void _create_program();
		void _create_kernel();
		void _release_pogram();
		void _release_kernel();

	public:
		ClKernel(
			std::weak_ptr<ClMachine> clMachine,
			std::string sourcePath,
			std::string kernelName,
			param_map params);
		ClKernel(
			std::weak_ptr<ClMachine> clMachine,
			std::filesystem::path sourcePath,
			std::string kernelName,
			param_map params);
		~ClKernel();

	public:
		cl_program clProgram() const;
		cl_kernel clKernel() const;

		// コピー禁止・ムーブ禁止
	public:
		ClKernel(const ClKernel&) = delete;
		ClKernel(ClKernel&&) = delete;
		ClKernel& operator=(const ClKernel&) = delete;
		ClKernel& operator=(ClKernel&&) = delete;
	};

}

#endif // !_MONJU_CL_KERNEL_H__
