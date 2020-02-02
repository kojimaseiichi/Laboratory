#pragma once
#ifndef _MONJU_CL_MEMORY_H__
#define _MONJU_CL_MEMORY_H__

#include "ClContext.h"


namespace monju {

	template <typename EigenType>
	class ClMemory
	{
	private:
		ClContext* _pClContext;
		EigenType* _pEigen;
		cl_mem _mem;	// ‰ð•ú—\’è

		cl_mem _createBuffer(cl_context context, size_t bytes, void* pData)
		{
			cl_int error;
			cl_mem mem = clCreateBuffer(
				context,
				CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
				bytes,
				pData,
				&error);
			if (error != CL_SUCCESS)
				throw OpenClException(error, "clCreateBuffer");
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
		ClMemory(ClContext& context, EigenType& eigen)
		{
			_pClContext = &context;
			_pEigen = &eigen;
			size_t bytes = sizeof(typename Matrix::Scalar) * eigen.size();
			_mem = _createBuffer(context.context(), bytes, eigen.data());
		}
		~ClMemory()
		{
			_releaseBuffer();
		}

		cl_mem clMem() const
		{
			return _mem;
		}

	};

}

#endif // !_MONJU_CL_MEMORY_H__
