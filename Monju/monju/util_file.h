// ファイルユーティリティ
#pragma once
#ifndef _MONJU_UTIL_FILE_H__
#define _MONJU_UTIL_FILE_H__

#include <string>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/serialization.hpp>

namespace monju {
	namespace util_file {

		// ファイルの存在チェック
		bool existsFile(const std::string& path);

		// ファイルのパスを取得
		std::string combine(const std::string& dir, const std::string& name, const std::string& extention);

		std::string combine(const std::string& dir, const std::string& name);

		// ファイルの内容を取得
		std::string readContent(const std::string& path);

		// シリアル化
		template <typename T>
		void serialize(const std::string& path, std::string root, T& obj)
		{
			std::ofstream file(path);
			boost::archive::text_oarchive ar(file);
			ar << boost::serialization::make_nvp(root.c_str(), obj);
		}

		// 逆シリアル化
		template <typename T>
		void deserialize(const std::string& path, std::string root, T& obj)
		{
			std::ifstream file(path);
			boost::archive::text_iarchive ar(file);
			ar >> boost::serialization::make_nvp(root.c_str(), obj);
		}
	}
}

#endif // _MONJU_UTIL_FILE_H__
