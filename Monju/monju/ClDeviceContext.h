#pragma once
#ifndef _MONJU_CL_DEVICE_CONTEXT_H__
#define _MONJU_CL_DEVICE_CONTEXT_H__

#include "MonjuTypes.h"

namespace monju
{
	class ClDeviceContext
	{
	private:
		std::shared_ptr<ClMachine> _clMachine;
		std::shared_ptr<_ClContext> _clContext; // ClMachineから取得
		cl_device_id _deviceId;
		std::unique_ptr<_ClCommandQueue> _commandQueue;	// 解放予定

		void _createCommandQueue();
		void _releaseCommandQueue();
		void _enqueueReadBuffer(std::weak_ptr<_ClBuffer> clBuffer, void* pData, ClEventJoiner* pEvent);
		void _enqueueWriteBuffer(std::weak_ptr<_ClBuffer> clBuffer, const void* pData, ClEventJoiner* pEvent);
		void _enqueueNDRangeKernel(
			std::weak_ptr<ClKernel> kernel,
			std::vector<size_t> globalWorkSize,
			std::vector<size_t>* pLocalWorkSize,
			ClEventJoiner* pEvent);

	public:
		ClDeviceContext(std::weak_ptr<ClMachine> clMachine, cl_device_id deviceId);
		~ClDeviceContext();
		void enqueueReadBuffer(std::weak_ptr<ClMemory> clMemory, void* pData, ClEventJoiner* pEvent);
		void readBuffer(std::weak_ptr<ClMemory> clMemory, void* pData);
		void enqueueWriteBuffer(std::weak_ptr<ClMemory> clMemory, void* pData, ClEventJoiner* pEvent);
		void writeBuffer(std::weak_ptr<ClMemory> clMemory, void* pData);
		void enqueueNDRangeKernel(
			std::weak_ptr<ClKernel> kernel,
			std::vector<size_t> globalWorkSize,
			std::vector<size_t> localWorkSize,
			ClEventJoiner* pEvent);
		void enqueueNDRangeKernel(
			std::weak_ptr<ClKernel> kernel,
			std::vector<size_t> globalWorkSize,
			ClEventJoiner* pEvent);
		void flush();
		void finish();

		// コピー禁止・ムーブ禁止
	public:
		ClDeviceContext(const ClDeviceContext&) = delete;
		ClDeviceContext(ClDeviceContext&&) = delete;
		ClDeviceContext& operator=(const ClDeviceContext&) = delete;
		ClDeviceContext& operator=(ClDeviceContext&&) = delete;
	};
}

#endif // !_MONJU_CL_DEVICE_CONTEXT_H__
