#include "_ClBuffer.h"
#include "OpenClException.h"

cl_mem monju::_ClBuffer::_createBuffer(cl_context context, size_t size)
{
	cl_int error;
	cl_mem mem = clCreateBuffer(
		context,
		CL_MEM_READ_WRITE,
		size,
		nullptr,
		&error);
	if (error != CL_SUCCESS)
		throw new OpenClException(error, "clCreateBuffer");
	return mem;
}

void monju::_ClBuffer::_releaseBuffer()
{
	if (_mem == nullptr)
		return;
	cl_int error = clReleaseMemObject(_mem);
	if (error != CL_SUCCESS)
		throw OpenClException(error, "clReleaseMemObject");
	_mem = nullptr;
}

monju::_ClBuffer::_ClBuffer(cl_context context, size_t size)
{
	_mem = _createBuffer(context, size);
	_size = size;
}

monju::_ClBuffer::~_ClBuffer()
{
	_releaseBuffer();
}

cl_mem monju::_ClBuffer::clMem() const
{
	return _mem;
}

size_t monju::_ClBuffer::size() const
{
	return _size;
}
