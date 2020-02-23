#pragma once
#ifndef _MONJU_CL_DEVICE_CONTEXT_H__
#define _MONJU_CL_DEVICE_CONTEXT_H__

#include "ClMachine.h"
#include "ClKernel.h"
#include "ClMemory.h"
#include "_ClCommandQueue.h"
#include "ClEventJoiner.h"

namespace monju
{
	class ClDeviceContext
	{
	private:
		std::shared_ptr<ClMachine> _clMachine;
		std::shared_ptr<_ClContext> _clContext; // ClMachineから取得
		cl_device_id _deviceId;
		std::unique_ptr<_ClCommandQueue> _commandQueue;	// 解放予定

		void _createCommandQueue()
		{
			_commandQueue = std::make_unique<_ClCommandQueue>(
				_clContext->context(),
				_deviceId);
		}
		void _releaseCommandQueue()
		{
			_commandQueue->flush();
			_commandQueue->finish();
			_commandQueue.reset();
		}
		void _enqueueReadBuffer(std::weak_ptr<_ClBuffer> clBuffer, void* pData, ClEventJoiner* pEvent)
		{
			auto pBuff = clBuffer.lock();

			cl_event ev = nullptr;
			cl_int error = clEnqueueReadBuffer(
				_commandQueue->clCommandQueue(),
				pBuff->clMem(),
				pEvent == nullptr,	// pEventがNULL場合は同期実行
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
		void _enqueueWriteBuffer(std::weak_ptr<_ClBuffer> clBuffer, const void* pData, ClEventJoiner* pEvent)
		{
			auto pBuff = clBuffer.lock();

			cl_event ev = nullptr;;
			cl_int error = clEnqueueWriteBuffer(
				_commandQueue->clCommandQueue(),
				pBuff->clMem(),
				pEvent == nullptr,	// pEventがNULL場合は同期実行
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
		void _enqueueNDRangeKernel(
			std::weak_ptr<ClKernel> kernel,
			std::vector<size_t> globalWorkSize,
			std::vector<size_t>* pLocalWorkSize,
			ClEventJoiner* pEvent)
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

	public:
		ClDeviceContext(std::weak_ptr<ClMachine> clMachine, cl_device_id deviceId)
		{
			_clMachine = clMachine.lock();
			_clContext = _clMachine->clContext().lock();
			_deviceId = deviceId;
			_createCommandQueue();
		}
		~ClDeviceContext()
		{
			_releaseCommandQueue();
		}
		void enqueueReadBuffer(std::weak_ptr<ClMemory> clMemory, void* pData, ClEventJoiner* pEvent)
		{
			auto mem = clMemory.lock();
			_enqueueReadBuffer(mem->clBuffer(), pData, pEvent);
			_commandQueue->flush();
		}
		void readBuffer(std::weak_ptr<ClMemory> clMemory, void* pData)
		{
			auto mem = clMemory.lock();
			_enqueueReadBuffer(mem->clBuffer(), pData, nullptr);
			_commandQueue->flush();
		}
		void enqueueWriteBuffer(std::weak_ptr<ClMemory> clMemory, void* pData, ClEventJoiner* pEvent)
		{
			auto mem = clMemory.lock();
			_enqueueWriteBuffer(mem->clBuffer(), pData, pEvent);
			_commandQueue->flush();
		}
		void writeBuffer(std::weak_ptr<ClMemory> clMemory, void* pData)
		{
			auto mem = clMemory.lock();
			_enqueueWriteBuffer(mem->clBuffer(), pData, nullptr);
			_commandQueue->flush();
		}
		void enqueueNDRangeKernel(
			std::weak_ptr<ClKernel> kernel,
			std::vector<size_t> globalWorkSize,
			std::vector<size_t> localWorkSize,
			ClEventJoiner* pEvent)
		{
			_enqueueNDRangeKernel(kernel, globalWorkSize, &localWorkSize, pEvent);
			_commandQueue->flush();
		}
		void enqueueNDRangeKernel(
			std::weak_ptr<ClKernel> kernel,
			std::vector<size_t> globalWorkSize,
			ClEventJoiner* pEvent)
		{
			_enqueueNDRangeKernel(kernel, globalWorkSize, nullptr, pEvent);
			_commandQueue->flush();
		}
		void flush()
		{
			_commandQueue->flush();
		}
		void finish()
		{
			_commandQueue->finish();
		}
		// コピー禁止・ムーブ禁止
	public:
		ClDeviceContext(const ClDeviceContext&) = delete;
		ClDeviceContext(ClDeviceContext&&) = delete;
		ClDeviceContext& operator=(const ClDeviceContext&) = delete;
		ClDeviceContext& operator=(ClDeviceContext&&) = delete;
	};
}

#endif // !_MONJU_CL_DEVICE_CONTEXT_H__
