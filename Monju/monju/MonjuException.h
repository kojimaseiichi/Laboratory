#pragma once
#ifndef _MONJU_MONJU_EXCEPTION_H__
#define _MONJU_MONJU_EXCEPTION_H__

#include "MonjuTypes.h"

namespace monju {

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