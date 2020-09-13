#pragma once
#ifndef _MONJU_CL_DEVICE_CONTEXT_H__
#define _MONJU_CL_DEVICE_CONTEXT_H__

#include <stdint.h>
#include <memory>
#include <vector>
#include <CL/cl.h>

namespace monju
{
	/* 前方宣言 */
	class _ClContext;
	class _ClCommandQueue;
	class _ClBuffer;
	class ClMachine;
	class ClEventJoiner;
	class ClKernel;
	class ClMemory;

	class ClDeviceContext
	{
	private:
		std::shared_ptr<ClMachine> _clMachine;
		std::shared_ptr<_ClContext> _clContext; // ClMachineから取得
		cl_device_id _deviceId;
		std::unique_ptr<_ClCommandQueue> _commandQueue;	// 解放予定

	private:
		void _create_command_queue();
		void _release_command_queue();
		void _enqueue_read_buffer(std::weak_ptr<_ClBuffer> clBuffer, void* pData, ClEventJoiner* pEvent);
		void _enqueue_write_buffer(std::weak_ptr<_ClBuffer> clBuffer, const void* pData, ClEventJoiner* pEvent);
		void _enqueue_nd_range_kernel(
			std::weak_ptr<ClKernel> kernel,
			std::vector<size_t> globalWorkSize,
			std::vector<size_t>* pLocalWorkSize,
			ClEventJoiner* pEvent);

	public:
		ClDeviceContext(std::weak_ptr<ClMachine> clMachine, cl_device_id deviceId);
		~ClDeviceContext();

		void read_buffer(std::weak_ptr<ClMemory> clMemory, void* pData);
		void write_buffer(std::weak_ptr<ClMemory> clMemory, void* pData);
		void enqueue_read_buffer(std::weak_ptr<ClMemory> clMemory, void* pData, ClEventJoiner* pEvent);
		void enqueue_write_buffer(std::weak_ptr<ClMemory> clMemory, void* pData, ClEventJoiner* pEvent);
		void enqueue_ndrange_kernel_to_execute(
			std::weak_ptr<ClKernel> kernel,
			std::vector<size_t> globalWorkSize,
			std::vector<size_t> localWorkSize,
			ClEventJoiner* pEvent);
		void enqueue_ndrange_kernel_to_execute(
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
