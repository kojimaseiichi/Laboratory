#pragma once
#ifndef _MONJU_CL_BUFFER_H__
#define _MONJU_CL_BUFFER_H__

#include <CL/cl.h>
#include "OpenClException.h"
#include "MonjuTypes.h"

namespace monju
{
	class ClBuffer
	{
	private:
		size_t _size;
		void* _p;
		cl_mem _mem;	// 解放予定

		cl_mem _createBuffer(cl_context context, size_t size, void* pBuff)
		{
			cl_int error;
			cl_mem mem = clCreateBuffer(
				context,
				CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
				size, pBuff,
				&error);
			if (error != CL_SUCCESS)
				throw new OpenClException(error, "clCreateBuffer");
			return mem;
		}
		void _releaseBuffer()
		{
			if (_mem == nullptr)
				return;
			cl_int error = clReleaseMemObject(_mem);
			if (error != CL_SUCCESS)
				throw OpenClException(error, "clReleaseMemObject");
			_mem = nullptr;
		}

	public:
		ClBuffer(cl_context context, size_t size, void* pBuff)
		{
			_mem = _createBuffer(context, size, pBuff);
			_size = size;
			_p = pBuff;
		}
		~ClBuffer()
		{
			_releaseBuffer();
		}
		cl_mem mem() const
		{
			return _mem;
		}
		size_t size() const
		{
			return _size;
		}
		void* ptr() const
		{
			return _p;
		}

		// コピー禁止・ムーブ禁止
	public:
		ClBuffer(const ClBuffer&) = delete;
		ClBuffer(ClBuffer&&) = delete;
		ClBuffer& operator=(const ClBuffer&) = delete;
		ClBuffer& operator=(ClBuffer&&) = delete;

	};
}

#endif // !_MONJU_CL_BUFFER_H__
