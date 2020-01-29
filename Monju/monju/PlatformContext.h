#pragma once
#ifndef _MONJU_PLATFORM_CONTEXT_H__
#define _MONJU_PLATFORM_CONTEXT_H__

#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/filesystem.hpp>

#include "monju/DeviceContext.h"

namespace monju {

	class PlatformContext
	{
	private:
		boost::filesystem::path _work_folder;
		boost::filesystem::path _kernel_folder;
		int _platform_id;

	private:
		std::unique_ptr<DeviceContext> _device_context;

	private:
		// プロパティファイルへのパス
		std::string full_path_to_prpperties_json(std::string work_folder) const;
		// プラットフォーム情報をJSON形式でファイルに書き込み
		void saveJson(const std::string work_folder) const;
		// プラットフォーム情報をファイルから読み込み
		void loadJson(const std::string work_folder);
		// 
		void createDeviceContext(int platform_id);



		// 生成・初期化
	public:
		PlatformContext();
		~PlatformContext();

		// 公開関数
	public:
		// プラットフォーム情報をファイルから読み込み利用可能とする
		void open(std::string work_folder);
		void create(std::string work_folder, std::string kernel_folder, int platform_id);
		// プラットフォーム情報をファイルに保存する
		void close();

		// プロパティ
	public:
		// ワークスペースディレクトリ
		std::string workspaceDir();
		std::string kernelDir();
		// デバイスコンテキスト（計算資源）
		DeviceContext& deviceContext() const;

	};

}

#endif // !_MONJU_PLATFORM_CONTEXT_H__
