#pragma once
#ifndef _MONJU_OPEN_CL_KERNEL_H__
#define _MONJU_OPEN_CL_KERNEL_H__

#include "ClContext.h"
#include "ClProgram.h"
#include "ClKernel.h"
#include "ClCommandQueues.h"
#include <memory>

namespace monju
{
	class OpenClKernel
	{
	private:
		ClContext* _pClContext;
		std::string _sourcePath;
		std::string _kernelName;
		std::map<std::string, std::string> _params;

		std::shared_ptr<ClProgram> _clProgram;	// âï˙ó\íË
		std::shared_ptr<ClKernel> _clKernel;	// âï˙ó\íË

		void _createProgram();
		void _createKernel();
		void _releaseProgram();
		void _releaseKernel();

	public:
		OpenClKernel(
			ClContext& clContext,
			std::string sourcePath,
			std::string kernelName,
			std::map<std::string, std::string> params);
		~OpenClKernel();
		std::weak_ptr<ClProgram> clProgram() const;
		std::weak_ptr<ClKernel> clKernel() const;

		// ÉRÉsÅ[ã÷é~ÅEÉÄÅ[Éuã÷é~
	public:
		OpenClKernel(const OpenClKernel&) = delete;
		OpenClKernel(OpenClKernel&&) = delete;
		OpenClKernel& operator=(const OpenClKernel&) = delete;
		OpenClKernel& operator=(OpenClKernel&&) = delete;
	};

}

#endif // !_MONJU_OPEN_CL_KERNEL_H__
