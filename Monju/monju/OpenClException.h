#pragma once
#ifndef _MONJU_OPEN_CL_EXCEPTION_H__
#define _MONJU_OPEN_CL_EXCEPTION_H__

#include "MonjuException.h"

namespace monju {

	class OpenClException : MonjuException
	{
	protected:
		int _opencl_error_code;

	public:
		OpenClException(int opencl_error_code);
		OpenClException(int opencl_error_code, std::string message);
		~OpenClException();

		// プロパティ
	public:
		int openClErrorCode() const { return _opencl_error_code; }
	};

} // namespace monju

#endif // !_MONJU_OPEN_CL_EXCEPTION_H__
