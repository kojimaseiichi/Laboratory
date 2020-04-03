#pragma once
#ifndef _MONJU_CL_FUNC_H__
#define _MONJU_CL_FUNC_H__

#include "MonjuTypes.h"
#include "ClKernel.h"

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
			ClEventJoiner* pEvent);

	public:
		ClFunc(std::weak_ptr<ClMachine> clMachine, std::weak_ptr<ClKernel> clKernel);
		~ClFunc();
		void pushArgument(float value);
		void pushArgument(ClMemory& clMemory);
		void pushArgument(std::weak_ptr<ClMemory> clMemory);
		void clearArguments();
		void execute(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			std::vector<size_t> globalWorkSize,
			std::vector<size_t> localWorkSize,
			ClEventJoiner* pEvent);
		void execute(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			std::vector<size_t> globalWorkSize,
			ClEventJoiner* pEvent);

		// コピー禁止・ムーブ禁止
	public:
		ClFunc(const ClFunc&) = delete;
		ClFunc(ClFunc&&) = delete;
		ClFunc& operator=(const ClFunc&) = delete;
		ClFunc& operator=(ClFunc&&) = delete;
	};
}

#endif // !_MONJU_CL_FUNC_H__
