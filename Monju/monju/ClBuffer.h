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
		cl_mem _mem;	// ‰ð•ú—\’è

		cl_mem _createBuffer(cl_context context, size_t size, void* pBuff)
		{
			cl_int error;
			cl_mem mem = clCreateBuffer(context, CL_MEM_READ_WRITE, size, pBuff, &error);
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
	};
}

#endif // !_MONJU_CL_BUFFER_H__
