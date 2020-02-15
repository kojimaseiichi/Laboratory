#pragma once
#ifndef _MONJU_CL_MEMORY_H__
#define _MONJU_CL_MEMORY_H__

#include "ClMachine.h"
#include "_ClContext.h"
#include "_ClBuffer.h"

namespace monju
{
	class ClMemory
	{
	protected:
		std::shared_ptr<ClMachine> _clMachine;
		std::shared_ptr<_ClContext> _clContext;	// ClMachineから取得
		size_t _size;

		std::shared_ptr<_ClBuffer> _clBuffer;	// 解放予定

		void _createBuffer()
		{
			_clBuffer = std::make_shared<_ClBuffer>(
				_clContext->context(),
				_size);
		}
		void _releaseBuffer()
		{
			bool unique = _clBuffer.use_count() == 1;
			_clBuffer.reset();
			if (!unique)
				throw MonjuException("_clBuffer");
		}

	public:
		ClMemory(std::weak_ptr<ClMachine> clMachine, size_t size)
		{
			_clMachine = clMachine.lock();
			_clContext = _clMachine->clContext().lock();
			_size = size;
			_createBuffer();
		}
		~ClMemory()
		{
			_releaseBuffer();
		}
		std::weak_ptr<_ClBuffer> clBuffer() const
		{
			return _clBuffer;
		}
		size_t size() const
		{
			return _size;
		}

		// コピー禁止・ムーブ禁止
	public:
		ClMemory(const ClMemory&) = delete;
		ClMemory(ClMemory&&) = delete;
		ClMemory& operator=(const ClMemory&) = delete;
		ClMemory& operator=(ClMemory&&) = delete;
	};

}

#endif // !_MONJU_CL_MEMORY_H__
