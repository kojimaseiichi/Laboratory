#pragma once
#ifndef _MONJU_OPEN_CL_MEMORY_H__
#define _MONJU_OPEN_CL_MEMORY_H__

#include "ClContext.h"
#include "ClBuffer.h"

namespace monju
{
	class OpenClMemory
	{
	protected:
		std::weak_ptr<ClContext> _clContext;
		size_t _size;
		void* _p;

		std::shared_ptr<ClBuffer> _clBuffer;	// ����\��

		void _createBuffer();
		void _releaseBuffer();

	public:
		OpenClMemory(std::weak_ptr<ClContext> clContext, size_t size, void* p);
		~OpenClMemory();
		std::weak_ptr<ClBuffer> clBuffer() const;

		// �R�s�[�֎~�E���[�u�֎~
	public:
		OpenClMemory(const OpenClMemory&) = delete;
		OpenClMemory(OpenClMemory&&) = delete;
		OpenClMemory& operator=(const OpenClMemory&) = delete;
		OpenClMemory& operator=(OpenClMemory&&) = delete;
	};

}

#endif // !_MONJU_OPEN_CL_MEMORY_H__
