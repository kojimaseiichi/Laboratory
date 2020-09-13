#include "_ClBuffer.h"
#include "OpenClException.h"

cl_mem monju::_ClBuffer::_create_buffer(cl_context context, size_t size)
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

void monju::_ClBuffer::_release_buffer()
{
	if (_mem_obj == nullptr)
		return;
	cl_int error = clReleaseMemObject(_mem_obj);
	if (error != CL_SUCCESS)
		throw OpenClException(error, "clReleaseMemObject");
	_mem_obj = nullptr;
}

monju::_ClBuffer::_ClBuffer(cl_context context, size_t size)
{
	_mem_obj = _create_buffer(context, size);
	_mem_bytes = size;
}

monju::_ClBuffer::~_ClBuffer()
{
	_release_buffer();
}

cl_mem monju::_ClBuffer::mem_obj() const
{
	return _mem_obj;
}

size_t monju::_ClBuffer::mem_size() const
{
	return _mem_bytes;
}
