#pragma once
#ifndef _MONJU_ENVIRONMENT_H__
#define _MONJU_ENVIRONMENT_H__

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/filesystem.hpp>

namespace monju
{
	class Environment
	{
	private:
		boost::filesystem::path _workFolder;
		boost::filesystem::path _kernelFolder;
		int _platformId;

		// プロパティファイルへのパス
		std::string _propertyFilePath(std::string work_folder) const
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
			root.put("kernel_folder", _kernelFolder.string());
			{
				// deivce
				boost::property_tree::ptree device;
				device.put("platform_id", _platformId);
				root.add_child("device", device);
			}
			boost::property_tree::write_json(_propertyFilePath(work_folder), root);
		}
		// プラットフォーム情報をファイルから読み込み
		void _loadJson(const std::string work_folder)
		{
			boost::property_tree::ptree root;
			boost::property_tree::read_json(_propertyFilePath(work_folder), root);
			// root
			_workFolder = work_folder;
			_kernelFolder = root.get<std::string>("kernel_folder");
			{
				// deivce
				auto device = root.get_child("device");
				_platformId = device.get<int>("platform_id");
			}
		}

	public:
		void open(std::string work_folder)
		{
			_workFolder = work_folder;
			_loadJson(work_folder);
		}
		int platformId() const
		{
			return _platformId;
		}
		std::string workspaceDir()
		{
			return _workFolder.string();
		}
		std::string kernelDir()
		{
			return _kernelFolder.string();
		}

		// コピー禁止・ムーブ禁止
	public:
		Environment(const Environment&) = delete;
		Environment(Environment&&) = delete;
		Environment& operator=(const Environment&) = delete;
		Environment& operator=(Environment&&) = delete;
	};
}

#endif // !_MONJU_ENVIRONMENT_H__
