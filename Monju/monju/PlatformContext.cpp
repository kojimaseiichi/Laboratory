#include "PlatformContext.h"

// プロパティファイルへのパス

// プラットフォーム情報をJSON形式でファイルに書き込み

// プラットフォーム情報をファイルから読み込み

monju::PlatformContext::PlatformContext()
{
	_work_folder = "";
	_platform_id = 0;
}

monju::PlatformContext::~PlatformContext()
{
	_release();
}

// プラットフォーム情報をファイルから読み込み利用可能とする

// プラットフォーム情報をファイルに保存する

// ワークスペースディレクトリ

// デバイスコンテキスト（計算資源）

monju::DeviceContext& monju::PlatformContext::deviceContext() const
{
	return *_pDeviceContext;
}
