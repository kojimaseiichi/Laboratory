#include "ClFunc.h"

#include "_ClBuffer.h"
#include "_ClCommandQueue.h"

// #include "ClKernel.h" ヘッダーに移動
#include "ClMachine.h"
#include "ClMemory.h"
#include "ClDeviceContext.h"
#include "ClEventJoiner.h"
#include "OpenClException.h"

void monju::ClFunc::_execute(ClDeviceContext* pDeviceContext, std::vector<size_t> globalWorkSize, std::vector<size_t>* pLocalWorkSize, ClEventJoiner* pEvent)
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
	if (pLocalWorkSize == nullptr)
		pDeviceContext->enqueueNDRangeKernel(
			_clKernel,
			globalWorkSize,
			pEvent);
	else
		pDeviceContext->enqueueNDRangeKernel(
			_clKernel,
			globalWorkSize,
			*pLocalWorkSize,
			pEvent);
}

monju::ClFunc::ClFunc(std::weak_ptr<ClMachine> clMachine, std::weak_ptr<ClKernel> clKernel)
{
	_clMachine = clMachine.lock();
	_clKernel = clKernel.lock();
}

monju::ClFunc::~ClFunc()
{

}

void monju::ClFunc::pushArgument(float value)
{
	Arg a;
	a.size = sizeof(float);
	a.argValue.vFloat = value;
	_args.push_back(a);
}

void monju::ClFunc::pushArgument(ClMemory& clMemory)
{
	auto p = clMemory.clBuffer().lock();
	Arg a;
	a.size = sizeof(cl_mem);
	a.argValue.clMem = p->clMem();
	_args.push_back(a);
}

void monju::ClFunc::pushArgument(std::weak_ptr<ClMemory> clMemory)
{
	auto p = clMemory.lock();
	pushArgument(*p);
}

void monju::ClFunc::clearArguments()
{
	_args.clear();
}

void monju::ClFunc::execute(std::weak_ptr<ClDeviceContext> clDeviceContext, std::vector<size_t> globalWorkSize, std::vector<size_t> localWorkSize, std::weak_ptr<ClEventJoiner> clEventJoiner)
{
	auto pdc = clDeviceContext.lock();
	auto pej = clEventJoiner.lock();
	_execute(pdc.get(), globalWorkSize, &localWorkSize, pej.get());
}

void monju::ClFunc::execute(std::weak_ptr<ClDeviceContext> clDeviceContext, std::vector<size_t> globalWorkSize, std::weak_ptr<ClEventJoiner> clEventJoiner)
{
	auto pdc = clDeviceContext.lock();
	auto pej = clEventJoiner.lock();
	_execute(pdc.get(), globalWorkSize, nullptr, pej.get());
}

