#include "ClDeviceContext.h"

#include "_ClContext.h"
#include "_ClCommandQueue.h"
#include "_ClBuffer.h"

#include "ClEventJoiner.h"
#include "ClMachine.h"
#include "ClMemory.h"
#include "ClKernel.h"
#include "OpenClException.h"

void monju::ClDeviceContext::_createCommandQueue()
{
	_commandQueue = std::make_unique<_ClCommandQueue>(
		_clContext->context(),
		_deviceId);
}

void monju::ClDeviceContext::_releaseCommandQueue()
{
	_commandQueue->flush();
	_commandQueue->finish();
	_commandQueue.reset();
}

void monju::ClDeviceContext::_enqueueReadBuffer(std::weak_ptr<monju::_ClBuffer> clBuffer, void* pData, monju::ClEventJoiner* pEvent)
{
	auto pBuff = clBuffer.lock();

	cl_event ev = nullptr;
	cl_int error = clEnqueueReadBuffer(
		_commandQueue->clCommandQueue(),
		pBuff->clMem(),
		pEvent == nullptr,	// pEvent‚ªNULLê‡‚Í“¯ŠúŽÀs
		0,
		pBuff->size(),
		pData,
		0,
		nullptr,
		pEvent == nullptr ? nullptr : &ev);
	if (errno != CL_SUCCESS)
		throw OpenClException(error, "clEnqueueReadBuffer");
	if (pEvent != nullptr)
		pEvent->push(ev);
}

void monju::ClDeviceContext::_enqueueWriteBuffer(std::weak_ptr<monju::_ClBuffer> clBuffer, const void* pData, monju::ClEventJoiner* pEvent)
{
	auto pBuff = clBuffer.lock();

	cl_event ev = nullptr;;
	cl_int error = clEnqueueWriteBuffer(
		_commandQueue->clCommandQueue(),
		pBuff->clMem(),
		pEvent == nullptr,	// pEvent‚ªNULLê‡‚Í“¯ŠúŽÀs
		0,
		pBuff->size(),
		pData,
		0,
		nullptr,
		pEvent == nullptr ? nullptr : &ev);
	if (errno != CL_SUCCESS)
		throw OpenClException(error, "clEnqueueWriteBuffer");
	if (pEvent != nullptr)
		pEvent->push(ev);
}

void monju::ClDeviceContext::_enqueueNDRangeKernel(std::weak_ptr<ClKernel> kernel, std::vector<size_t> globalWorkSize, std::vector<size_t>* pLocalWorkSize, ClEventJoiner* pEvent)
{
	auto pKernel = kernel.lock();
	cl_event ev = nullptr;
	cl_int error = clEnqueueNDRangeKernel(
		_commandQueue->clCommandQueue(),
		pKernel->clKernel(),
		globalWorkSize.size(),
		nullptr,
		globalWorkSize.data(),
		pLocalWorkSize != nullptr ? pLocalWorkSize->data() : nullptr,
		0,
		nullptr,
		pEvent == nullptr ? nullptr : &ev);
	if (error != CL_SUCCESS)
		throw OpenClException(error, "clEnqueueNDRangeKernel");
	if (pEvent != nullptr)
		pEvent->push(ev);
}

monju::ClDeviceContext::ClDeviceContext(std::weak_ptr<ClMachine> clMachine, cl_device_id deviceId)
{
	_clMachine = clMachine.lock();
	_clContext = _clMachine->clContext().lock();
	_deviceId = deviceId;
	_createCommandQueue();
}

monju::ClDeviceContext::~ClDeviceContext()
{
	_releaseCommandQueue();
}

void monju::ClDeviceContext::enqueueReadBuffer(std::weak_ptr<ClMemory> clMemory, void* pData, ClEventJoiner* pEvent)
{
	auto mem = clMemory.lock();
	_enqueueReadBuffer(mem->clBuffer(), pData, pEvent);
	_commandQueue->flush();
}

void monju::ClDeviceContext::readBuffer(std::weak_ptr<ClMemory> clMemory, void* pData)
{
	auto mem = clMemory.lock();
	_enqueueReadBuffer(mem->clBuffer(), pData, nullptr);
	_commandQueue->flush();
}

void monju::ClDeviceContext::enqueueWriteBuffer(std::weak_ptr<ClMemory> clMemory, void* pData, ClEventJoiner* pEvent)
{
	auto mem = clMemory.lock();
	_enqueueWriteBuffer(mem->clBuffer(), pData, pEvent);
	_commandQueue->flush();
}

void monju::ClDeviceContext::writeBuffer(std::weak_ptr<ClMemory> clMemory, void* pData)
{
	auto mem = clMemory.lock();
	_enqueueWriteBuffer(mem->clBuffer(), pData, nullptr);
	_commandQueue->flush();
}

void monju::ClDeviceContext::enqueueNDRangeKernel(std::weak_ptr<ClKernel> kernel, std::vector<size_t> globalWorkSize, std::vector<size_t> localWorkSize, ClEventJoiner* pEvent)
{
	_enqueueNDRangeKernel(kernel, globalWorkSize, &localWorkSize, pEvent);
	_commandQueue->flush();
}

void monju::ClDeviceContext::enqueueNDRangeKernel(std::weak_ptr<ClKernel> kernel, std::vector<size_t> globalWorkSize, ClEventJoiner* pEvent)
{
	_enqueueNDRangeKernel(kernel, globalWorkSize, nullptr, pEvent);
	_commandQueue->flush();
}

void monju::ClDeviceContext::flush()
{
	_commandQueue->flush();
}

void monju::ClDeviceContext::finish()
{
	_commandQueue->finish();
}
