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
		std::string full_path_to_prpperties_json(std::string work_folder) const
		{
			boost::filesystem::path bwf = boost::filesystem::path(work_folder);
			boost::filesystem::path full_path = bwf / "properties.json";
			return full_path.string();
		}
		// プラットフォーム情報をJSON形式でファイルに書き込み
		void _saveJson(const std::string work_folder) const
		{
			boost::property_tree::ptree root;
			// root
			root.put("work_folder", work_folder);
			root.put("kernel_folder", _kernel_folder.string());
			{
				// deivce
				boost::property_tree::ptree device;
				device.put("platform_id", _platform_id);
				root.add_child("device", device);
			}
			boost::property_tree::write_json(full_path_to_prpperties_json(work_folder), root);
		}
		// プラットフォーム情報をファイルから読み込み
		void _loadJson(const std::string work_folder)
		{
			boost::property_tree::ptree root;
			boost::property_tree::read_json(full_path_to_prpperties_json(work_folder), root);
			// root
			_work_folder = work_folder;
			_kernel_folder = root.get<std::string>("kernel_folder");
			{
				// deivce
				auto device = root.get_child("device");
				_platform_id = device.get<int>("platform_id");
			}
		}
		void _createDeviceContext(int platform_id)
		{
			_pDeviceContext = new DeviceContext();
			_pDeviceContext->create(platform_id);
		}
		void _release()
		{
			if (_pDeviceContext != nullptr)
			{
				_pDeviceContext->release();
				delete _pDeviceContext;
				_pDeviceContext = nullptr;
			}
		}

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
		void open(std::string work_folder)
		{
			_work_folder = work_folder;
			_loadJson(work_folder);
			_createDeviceContext(_platform_id);
		}
		void create(std::string work_folder, std::string kernel_folder, int platform_id)
		{
			_work_folder = work_folder;
			_kernel_folder = kernel_folder;
			_platform_id = platform_id;
			_saveJson(work_folder);
			_createDeviceContext(_platform_id);
		}
		// プラットフォーム情報をファイルに保存する
		void close()
		{
			_saveJson(_work_folder.string());
			_release();
		}

		// プロパティ
	public:
		// ワークスペースディレクトリ
		std::string workspaceDir()
		{
			return _work_folder.string();
		}
		std::string kernelDir()
		{
			return _kernel_folder.string();
		}
		// デバイスコンテキスト（計算資源）
		DeviceContext& deviceContext() const;

	};

}

#endif // !_MONJU_PLATFORM_CONTEXT_H__
