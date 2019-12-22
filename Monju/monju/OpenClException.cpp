#include "OpenClException.h"

monju::OpenClException::OpenClException(int opencl_error_code)
{
	_opencl_error_code = opencl_error_code;
}

monju::OpenClException::OpenClException(int opencl_error_code, std::string message) : MonjuException(message)
{
	_opencl_error_code = opencl_error_code;
}

monju::OpenClException::~OpenClException()
{
}
