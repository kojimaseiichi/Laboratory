#pragma once
#ifndef _MONJU_CL_FUNC_H__
#define _MONJU_CL_FUNC_H__

#include <vector>
#include <memory>
#include <CL/cl.h>

namespace monju
{
	/* forward decleration */
	class ClDeviceContext;
	class ClMachine;
	class ClMemory;
	class ClKernel;
	class ClEventJoiner;

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

	private:
		void _execute(
			ClDeviceContext* pDeviceContext,
			std::vector<size_t> globalWorkSize,
			std::vector<size_t>* pLocalWorkSize,
			ClEventJoiner* pEvent);

	public:
		ClFunc(std::weak_ptr<ClMachine> clMachine, std::weak_ptr<ClKernel> clKernel);
		~ClFunc();

	public:
		void push_kernel_argument(float value);
		void push_kernel_argument(ClMemory& clMemory);
		void push_kernel_argument(std::weak_ptr<ClMemory> clMemory);
		void clear_kernel_arguments();
		void execute_kernel(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			std::vector<size_t> globalWorkSize,
			std::vector<size_t> localWorkSize,
			std::weak_ptr<ClEventJoiner> clEventJoiner);
		void execute_kernel(
			std::weak_ptr<ClDeviceContext> clDeviceContext,
			std::vector<size_t> globalWorkSize,
			std::weak_ptr<ClEventJoiner> clEventJoiner);

		// コピー禁止・ムーブ禁止
	public:
		ClFunc(const ClFunc&) = delete;
		ClFunc(ClFunc&&) = delete;
		ClFunc& operator=(const ClFunc&) = delete;
		ClFunc& operator=(ClFunc&&) = delete;
	};
}

#endif // !_MONJU_CL_FUNC_H__
