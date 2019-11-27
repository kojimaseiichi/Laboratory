// ファイルユーティリティ
#pragma once
#ifndef _MONJU_UTIL_FILE_H__
#define _MONJU_UTIL_FILE_H__

#include <string>

namespace monju {
	namespace util_file {

		// ファイルの存在チェック
		bool existsFile(const std::string& path);

		// ファイルのパスを取得
		std::string combine(const std::string& dir, const std::string& name, const std::string& extention);

		// ファイルの内容を取得
		std::string readContent(const std::string& path);
	}
}

#endif // _MONJU_UTIL_FILE_H__
