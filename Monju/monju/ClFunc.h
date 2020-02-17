#pragma once
#ifndef _MONJU_OPEN_CL_KERNEL_EXECUTION_H__
#define _MONJU_OPEN_CL_KERNEL_EXECUTION_H__

#include "ClMachine.h"
#include "ClKernel.h"
#include "ClMemory.h"
#include "ClDeviceContext.h"
#include "ClEventJoiner.h"
#include "_ClCommandQueue.h"

namespace monju
{
	class ClFunc
	{
	private:
		union ArgValue
		{
			float vFloat;
			cl_mem clMem;
		};
		struct Arg
		{
			size_t size;
			ArgValue argValue;
		};
	private:
		std::vector<Arg> _args;
		std::shared_ptr<ClMachine> _clMachine;
		std::shared_ptr<ClKernel> _clKernel;

		void _execute(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			std::vector<size_t> globalWorkSize,
			std::vector<size_t>* pLocalWorkSize,
			ClEventJoiner* pEvent)
		{
			// 引数の設定
			for (int n = 0; n < _args.size(); n++)
			{
				const auto& r = _args.at(n);
				cl_int error = clSetKernelArg(_clKernel->clKernel(), n, r.size, &(r.argValue));
				if (error != CL_SUCCESS)
					throw OpenClException(error, "clSetKernelArg");
			}
			cl_event ev;
			// キューに追加
			auto p = clDeviceContext.lock();
			if (pLocalWorkSize == nullptr)
				p->enqueueNDRangeKernel(
					_clKernel,
					globalWorkSize,
					pEvent);
			else
				p->enqueueNDRangeKernel(
					_clKernel,
					globalWorkSize,
					*pLocalWorkSize,
					pEvent);
		}

	public:
		ClFunc(std::weak_ptr<ClMachine> clMachine, std::weak_ptr<ClKernel> clKernel)
		{
			_clMachine = clMachine.lock();
			_clKernel = clKernel.lock();
		}
		~ClFunc()
		{

		}
		void pushArgument(float value)
		{
			Arg a;
			a.size = sizeof(float);
			a.argValue.vFloat = value;
			_args.push_back(a);
		}
		void pushArgument(ClMemory& clMemory)
		{
			auto p = clMemory.clBuffer().lock();
			Arg a;
			a.size = sizeof(cl_mem);
			a.argValue.clMem = p->clMem();
			_args.push_back(a);
		}
		void pushArgument(std::weak_ptr<ClMemory> clMemory)
		{
			auto p = clMemory.lock();
			pushArgument(*p);
		}
		void clearArguments()
		{
			_args.clear();
		}
		void execute(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			std::vector<size_t> globalWorkSize,
			std::vector<size_t> localWorkSize,
			ClEventJoiner* pEvent)
		{
			_execute(clDeviceContext, globalWorkSize, &localWorkSize, pEvent);
		}
		void execute(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			std::vector<size_t> globalWorkSize,
			ClEventJoiner* pEvent)
		{
			_execute(clDeviceContext, globalWorkSize, nullptr, pEvent);
		}
		// コピー禁止・ムーブ禁止
	public:
		ClFunc(const ClFunc&) = delete;
		ClFunc(ClFunc&&) = delete;
		ClFunc& operator=(const ClFunc&) = delete;
		ClFunc& operator=(ClFunc&&) = delete;
	};
}

#endif // !_MONJU_OPEN_CL_KERNEL_EXECUTION_H__
