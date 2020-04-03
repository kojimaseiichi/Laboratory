#pragma once
#ifndef _MONJU_ENVIRONMENT_H__
#define _MONJU_ENVIRONMENT_H__

#include "MonjuTypes.h"

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
		std::string _propertyFilePath(std::string workFolder) const;
		// プラットフォーム情報をJSON形式でファイルに書き込み
		void _saveJson(const std::string workFolder) const;
		// プラットフォーム情報をファイルから読み込み
		void _loadJson(const std::string workFolder);

	public:
		Environment(std::string workFolder);
		~Environment();
		void open(std::string workFolder);
		Info info() const;

		// コピー禁止・ムーブ禁止
	public:
		Environment(const Environment&) = delete;
		Environment(Environment&&) = delete;
		Environment& operator=(const Environment&) = delete;
		Environment& operator=(Environment&&) = delete;
	};
}

#endif // !_MONJU_ENVIRONMENT_H__
