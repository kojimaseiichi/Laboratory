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
		DeviceContext* _pDeviceContext;

	private:
		// プロパティファイルへのパス
		std::string full_path_to_prpperties_json(std::string work_folder) const;
		// プラットフォーム情報をJSON形式でファイルに書き込み
		void _saveJson(const std::string work_folder) const;
		// プラットフォーム情報をファイルから読み込み
		void _loadJson(const std::string work_folder);
		void _createDeviceContext(int platform_id);
		void _release();

		// コピー禁止・ムーブ禁止
	public:
		PlatformContext(const PlatformContext&) = delete;
		PlatformContext(PlatformContext&&) = delete;
		PlatformContext& operator=(const PlatformContext&) = delete;
		PlatformContext& operator=(PlatformContext&&) = delete;

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
