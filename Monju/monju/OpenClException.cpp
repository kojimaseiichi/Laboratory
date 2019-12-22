#include "OpenClException.h"

inline monju::OpenClException::OpenClException(int opencl_error_code)
{
	_opencl_error_code = opencl_error_code;
}
