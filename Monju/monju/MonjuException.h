#pragma once
#ifndef _MONJU_MONJU_EXCEPTION_H__
#define _MONJU_MONJU_EXCEPTION_H__

#include <string>

namespace monju {

	/* ëOï˚êÈåæ */
	enum class ErrorCode;

	class MonjuException
	{
	protected:
		ErrorCode		_code;
		std::string		_message;

	public:
		MonjuException();
		MonjuException(std::string message);
		MonjuException(ErrorCode code);
		MonjuException(ErrorCode code, std::string message);
		~MonjuException();

	public:
		ErrorCode code() const { return _code; }
		std::string message() const { return _message; }

	}; // class MonjuException

} // namespace monju

#endif // _MONJU_MONJU_EXCEPTION_H__