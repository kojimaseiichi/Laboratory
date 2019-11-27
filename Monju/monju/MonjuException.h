#pragma once
#ifndef _MONJU_MONJU_EXCEPTION_H__
#define _MONJU_MONJU_EXCEPTION_H__

#include <string>

namespace monju {

	/// <summary>ライブラリの例外クラス</summary>
	class MonjuException
	{
	protected:
		std::string		_message;

		// 初期化・生成
	public:
		MonjuException();
		MonjuException(std::string message);
		~MonjuException();

	}; // class MonjuException

} // namespace monju

#endif // _MONJU_MONJU_EXCEPTION_H__