#pragma once
#ifndef _MONJU_MONJU_EXCEPTION_H__
#define _MONJU_MONJU_EXCEPTION_H__

#include <string>

namespace monju {

	enum class ErrorCode
	{
		Error = 0,
		WeakPointerExpired = 1
	};

	/// <summary>ライブラリの例外クラス</summary>
	class MonjuException
	{
	protected:
		ErrorCode		_code;
		std::string		_message;

		// 初期化・生成
	public:
		MonjuException();
		MonjuException(std::string message);
		MonjuException(ErrorCode code);
		MonjuException(ErrorCode code, std::string message);
		~MonjuException();

		ErrorCode code() const { return _code; }
		std::string message() const { return _message; }

	}; // class MonjuException

} // namespace monju

#endif // _MONJU_MONJU_EXCEPTION_H__