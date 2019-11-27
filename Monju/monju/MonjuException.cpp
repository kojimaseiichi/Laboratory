#include "MonjuException.h"


monju::MonjuException::MonjuException()
{
}

monju::MonjuException::MonjuException(std::string message)
{
	_message = message;
}


monju::MonjuException::~MonjuException()
{
}
