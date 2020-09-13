#pragma once
#ifndef _MONJU_CL_MEMORY_H__
#define _MONJU_CL_MEMORY_H__

#include <memory>
#include <stdint.h>

namespace monju
{
	class ClMachine;
	class _ClContext;
	class _ClBuffer;

	class ClMemory
	{
	protected:
		std::shared_ptr<ClMachine> _clMachine;
		std::shared_ptr<_ClContext> _clContext;	// ClMachine����擾
		size_t _size;

		std::shared_ptr<_ClBuffer> _clBuffer;	// ����\��

		void _createBuffer();
		void _releaseBuffer();

	public:
		ClMemory(std::weak_ptr<ClMachine> clMachine, size_t size);
		~ClMemory();
		std::weak_ptr<_ClBuffer> clBuffer() const;
		size_t size() const;

		// �R�s�[�֎~�E���[�u�֎~
	public:
		ClMemory(const ClMemory&) = delete;
		ClMemory(ClMemory&&) = delete;
		ClMemory& operator=(const ClMemory&) = delete;
		ClMemory& operator=(ClMemory&&) = delete;
	};

}

#endif // !_MONJU_CL_MEMORY_H__
