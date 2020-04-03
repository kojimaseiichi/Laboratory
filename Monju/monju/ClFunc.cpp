#include "ClFunc.h"

#include "_ClBuffer.h"
#include "_ClCommandQueue.h"

// #include "ClKernel.h" �w�b�_�[�Ɉړ�
#include "ClMachine.h"
#include "ClMemory.h"
#include "ClDeviceContext.h"
#include "ClEventJoiner.h"
#include "OpenClException.h"

void monju::ClFunc::_execute(std::weak_ptr<ClDeviceContext> clDeviceContext, std::vector<size_t> globalWorkSize, std::vector<size_t>* pLocalWorkSize, ClEventJoiner* pEvent)
{
	// �����̐ݒ�
	for (int n = 0; n < _args.size(); n++)
	{
		const auto& r = _args.at(n);
		cl_int error = clSetKernelArg(_clKernel->clKernel(), n, r.size, &(r.argValue));
		if (error != CL_SUCCESS)
			throw OpenClException(error, "clSetKernelArg");
	}
	cl_event ev;
	// �L���[�ɒǉ�
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

void monju::ClFunc::execute(std::weak_ptr<ClDeviceContext> clDeviceContext, std::vector<size_t> globalWorkSize, std::vector<size_t> localWorkSize, ClEventJoiner* pEvent)
{
	_execute(clDeviceContext, globalWorkSize, &localWorkSize, pEvent);
}

void monju::ClFunc::execute(std::weak_ptr<ClDeviceContext> clDeviceContext, std::vector<size_t> globalWorkSize, ClEventJoiner* pEvent)
{
	_execute(clDeviceContext, globalWorkSize, nullptr, pEvent);
}