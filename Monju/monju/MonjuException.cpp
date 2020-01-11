#include "MonjuException.h"


monju::MonjuException::MonjuException()
{
}

monju::MonjuException::MonjuException(std::string message)
{
	_code = ErrorCode::Error;
	_message = message;
}

monju::MonjuException::MonjuException(ErrorCode code)
{
	_code = code;
	_message = "";
}

monju::MonjuException::MonjuException(ErrorCode code, std::string message)
{
	_code = code;
	_message = message;
}

monju::MonjuException::~MonjuException()
{
}
