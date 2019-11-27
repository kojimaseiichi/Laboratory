#pragma once
#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/filesystem.hpp>

#ifndef _MONJU_PLATFORM_CONTEXT_H__
#define _MONJU_PLATFORM_CONTEXT_H__

namespace monju {

	class PlatformContext
	{
	private:
		const char* kJsonWorkFolder = "work_folder";

	private:
		boost::filesystem::path _work_folder;


	private:
		// プロパティファイルへのパス
		std::string full_path_to_prpperties_json() const
		{
			boost::filesystem::path full_path = _work_folder / "properties.json";
			return full_path.string();
		}
		// プラットフォーム情報をJSON形式でファイルに書き込み
		void saveJson() const
		{
			boost::property_tree::ptree pt;
			pt.put(kJsonWorkFolder, _work_folder);

			boost::property_tree::write_json(full_path_to_prpperties_json(), pt);
		}
		// プラットフォーム情報をファイルから読み込み
		void loadJson()
		{
			boost::property_tree::ptree pt;
			boost::property_tree::read_json(full_path_to_prpperties_json(), pt);

			//_work_folder = pt.get<std::string>(kJsonWorkFolder);
		}



		// 生成・初期化
	public:
		PlatformContext()
		{
		}
		~PlatformContext()
		{
		}

		// 公開関数
	public:
		// プラットフォーム情報をファイルから読み込み利用可能とする
		void open(std::string work_folder)
		{
			_work_folder = work_folder;
			loadJson();
		}
		// プラットフォーム情報をファイルに保存する
		void close()
		{
			saveJson();
		}
	};

}

#endif // !_MONJU_PLATFORM_CONTEXT_H__
