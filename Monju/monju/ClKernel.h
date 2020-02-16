#pragma once
#ifndef _MONJU_CL_KERNEL_H__
#define _MONJU_CL_KERNEL_H__

#include "ClMachine.h"
#include "_ClProgram.h"
#include "_ClKernel.h"
#include "_ClCommandQueue.h"
#include <memory>
#include <filesystem>
#include "MonjuTypes.h"

namespace monju
{
	class ClKernel
	{
	private:
		std::shared_ptr<ClMachine> _clMachine;
		std::shared_ptr<_ClContext> _clContext;	// ClMachineÇ©ÇÁéÊìæ
		std::string _sourcePath;
		std::string _kernelName;
		std::map<std::string, std::string> _params;

		std::shared_ptr<_ClProgram> _clProgram;	// âï˙ó\íË
		std::shared_ptr<_ClKernel> _clKernel;	// âï˙ó\íË

		void _createProgram()
		{
			_clProgram = std::make_shared<_ClProgram>(
				_clContext->context(),
				_clContext->deviceIds(),
				_sourcePath,
				_params);
		}
		void _createKernel()
		{
			_clKernel = std::make_shared<_ClKernel>(
				_clProgram->clProgram(),
				_kernelName,
				_params);
		}
		void _releaseProgram()
		{

			bool unique = _clProgram.use_count() == 1;
			_clProgram.reset();
			if (!unique)
				throw MonjuException("_clProgram");
		}
		void _releaseKernel()
		{
			bool unique = _clKernel.use_count() == 1;
			_clKernel.reset();
			if (!unique)
				throw MonjuException("_clKernel");
		}

	public:
		ClKernel(
			std::weak_ptr<ClMachine> clMachine,
			std::string sourcePath,
			std::string kernelName,
			params params)
		{
			_clMachine = clMachine.lock();
			_clContext = _clMachine->clContext().lock();
			_sourcePath = sourcePath;
			_kernelName = kernelName;
			_params = params;

			_createProgram();
			_createKernel();
		}
		ClKernel(
			std::weak_ptr<ClMachine> clMachine,
			std::filesystem::path sourcePath,
			std::string kernelName,
			params params)
		{
			_clMachine = clMachine.lock();
			_clContext = _clMachine->clContext().lock();
			_sourcePath = sourcePath.string();
			_kernelName = kernelName;
			_params = params;

			_createProgram();
			_createKernel();
		}
		~ClKernel()
		{
			_releaseKernel();
			_releaseProgram();
		}
		cl_program clProgram() const
		{
			return _clProgram->clProgram();
		}
		cl_kernel clKernel() const
		{
			return _clKernel->clKernel();
		}

		// ÉRÉsÅ[ã÷é~ÅEÉÄÅ[Éuã÷é~
	public:
		ClKernel(const ClKernel&) = delete;
		ClKernel(ClKernel&&) = delete;
		ClKernel& operator=(const ClKernel&) = delete;
		ClKernel& operator=(ClKernel&&) = delete;
	};

}

#endif // !_MONJU_CL_KERNEL_H__
