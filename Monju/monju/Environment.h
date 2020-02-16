#pragma once
#ifndef _MONJU_ENVIRONMENT_H__
#define _MONJU_ENVIRONMENT_H__

#include <string>
#include <fstream>
#include <boost/filesystem.hpp>
#include "nlohmann/json.hpp"

namespace monju
{
	class Environment
	{
	public:
		struct Info
		{
			std::string workFolder;
			std::string kernelFolder;
			int platformId;
		};
	private:
		Info _info;
		std::string _workFolder;

		// プロパティファイルへのパス
		std::string _propertyFilePath(std::string workFolder) const
		{
			boost::filesystem::path bwf = boost::filesystem::path(workFolder);
			boost::filesystem::path full_path = bwf / "properties.json";
			return full_path.string();
		}
		// プラットフォーム情報をJSON形式でファイルに書き込み
		void _saveJson(const std::string workFolder) const
		{
			std::string fileName = _propertyFilePath(workFolder);
			std::ofstream o;
			o.open(fileName, std::ofstream::out | std::ofstream::binary);
			nlohmann::json j;
			j["workFolder"] = _info.workFolder;
			j["kernelFolder"] = _info.kernelFolder;
			j["platformId"] = _info.platformId;
			o << j;
		}
		// プラットフォーム情報をファイルから読み込み
		void _loadJson(const std::string workFolder)
		{
			std::string fileName = _propertyFilePath(workFolder);
			std::ifstream i;
			i.open(fileName, std::ofstream::in | std::ofstream::binary);
			nlohmann::json j;
			i >> j;
			_info = {
				j["workFolder"].get<std::string>(),
				j["kernelFolder"].get<std::string>(),
				j["platformId"].get<int>()
			};
		}

	public:
		Environment(std::string workFolder)
		{
			open(workFolder);
		}
		~Environment()
		{
		}
		void open(std::string workFolder)
		{
			_workFolder = workFolder;
			_loadJson(workFolder);
		}
		Info info() const
		{
			return _info;
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
