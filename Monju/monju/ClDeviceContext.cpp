#include "ClDeviceContext.h"

#include "_ClContext.h"
#include "_ClCommandQueue.h"
#include "_ClBuffer.h"

#include "ClEventJoiner.h"
#include "ClMachine.h"
#include "ClMemory.h"
#include "ClKernel.h"
#include "OpenClException.h"

void monju::ClDeviceContext::_create_command_queue()
{
	_commandQueue = std::make_unique<_ClCommandQueue>(
		_clContext->context(),
		_deviceId);
}

void monju::ClDeviceContext::_release_command_queue()
{
	_commandQueue->flush();
	_commandQueue->finish();
	_commandQueue.reset();
}

void monju::ClDeviceContext::_enqueue_read_buffer(std::weak_ptr<monju::_ClBuffer> clBuffer, void* pData, monju::ClEventJoiner* pEvent)
{
	auto pBuff = clBuffer.lock();

	cl_event ev = nullptr;
	cl_int error = clEnqueueReadBuffer(
		_commandQueue->clCommandQueue(),	// command_queue
		pBuff->mem_obj(),	// buffer
		pEvent == nullptr,	// blocking_read pEvent‚ªNULLê‡‚Í“¯ŠúŽÀs
		0,					// offset
		pBuff->mem_size(),	// size
		pData,				// ptr
		0,					// num_events_in_wait_list
		nullptr,			// event_wait_list
		pEvent == nullptr ? nullptr : &ev	// event
	);
	if (errno != CL_SUCCESS)
		throw OpenClException(error, "clEnqueueReadBuffer");
	if (pEvent != nullptr)
		pEvent->push(ev);
}

void monju::ClDeviceContext::_enqueue_write_buffer(std::weak_ptr<monju::_ClBuffer> clBuffer, const void* pData, monju::ClEventJoiner* pEvent)
{
	auto pBuff = clBuffer.lock();

	cl_event ev = nullptr;;
	cl_int error = clEnqueueWriteBuffer(
		_commandQueue->clCommandQueue(),	// command_queue
		pBuff->mem_obj(),	// buffer
		pEvent == nullptr,	// blocking_read pEvent‚ªNULLê‡‚Í“¯ŠúŽÀs
		0,					// offset
		pBuff->mem_size(),	// size
		pData,				// ptr
		0,					// num_events_in_wait_list
		nullptr,			// event_wait_list
		pEvent == nullptr ? nullptr : &ev	// event
	);
	if (errno != CL_SUCCESS)
		throw OpenClException(error, "clEnqueueWriteBuffer");
	if (pEvent != nullptr)
		pEvent->push(ev);
}

void monju::ClDeviceContext::_enqueue_nd_range_kernel(std::weak_ptr<ClKernel> kernel, std::vector<size_t> globalWorkSize, std::vector<size_t>* pLocalWorkSize, ClEventJoiner* pEvent)
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
	_create_command_queue();
}

monju::ClDeviceContext::~ClDeviceContext()
{
	_release_command_queue();
}

void monju::ClDeviceContext::enqueue_read_buffer(std::weak_ptr<ClMemory> clMemory, void* pData, ClEventJoiner* pEvent)
{
	auto mem = clMemory.lock();
	_enqueue_read_buffer(mem->clBuffer(), pData, pEvent);
	_commandQueue->flush();
}

void monju::ClDeviceContext::read_buffer(std::weak_ptr<ClMemory> clMemory, void* pData)
{
	auto mem = clMemory.lock();
	_enqueue_read_buffer(mem->clBuffer(), pData, nullptr);
	_commandQueue->flush();
}

void monju::ClDeviceContext::enqueue_write_buffer(std::weak_ptr<ClMemory> clMemory, void* pData, ClEventJoiner* pEvent)
{
	auto mem = clMemory.lock();
	_enqueue_write_buffer(mem->clBuffer(), pData, pEvent);
	_commandQueue->flush();
}

void monju::ClDeviceContext::write_buffer(std::weak_ptr<ClMemory> clMemory, void* pData)
{
	auto mem = clMemory.lock();
	_enqueue_write_buffer(mem->clBuffer(), pData, nullptr);
	_commandQueue->flush();
}

void monju::ClDeviceContext::enqueue_ndrange_kernel_to_execute(std::weak_ptr<ClKernel> kernel, std::vector<size_t> globalWorkSize, std::vector<size_t> localWorkSize, ClEventJoiner* pEvent)
{
	_enqueue_nd_range_kernel(kernel, globalWorkSize, &localWorkSize, pEvent);
	_commandQueue->flush();
}

void monju::ClDeviceContext::enqueue_ndrange_kernel_to_execute(std::weak_ptr<ClKernel> kernel, std::vector<size_t> globalWorkSize, ClEventJoiner* pEvent)
{
	_enqueue_nd_range_kernel(kernel, globalWorkSize, nullptr, pEvent);
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
