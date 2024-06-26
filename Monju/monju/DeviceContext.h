#pragma once
#ifndef _MONJU_DEVICE_CONTEXT_H__
#define _MONJU_DEVICE_CONTEXT_H__

#include "MonjuTypes.h"

namespace monju {

	// 計算デバイスのコンテキスト
	// 一つのOpenCLプラットフォームを保持
	// 複数のOpenCLデバイスを所有
	class DeviceContext
	{
	private:
		cl_platform_id				_platform_id;
		std::vector<cl_device_id>	_device_id_vec;
		cl_context					_context;
		std::vector<Device*>		_device_vec;

		// 生成・初期化
	public:
		DeviceContext();
		~DeviceContext();

	private:
		cl_platform_id				_findPlatform(int platform_idx);
		std::vector<cl_device_id>	_listDevices(cl_platform_id platform_id);
		void						_releaseDevices(std::vector<cl_device_id>* p_device_id_vec);
		cl_context					_create_context(cl_platform_id platform_id, std::vector<cl_device_id>& device_id_vec);
		std::vector<Device*>		_allocDeviceVec(cl_context context, std::vector<cl_device_id> device_id_vec);
		void						_freeDeviceVec(std::vector<Device*>* p_device_vec);


	public:
		// OpenCLプラットフォーム初期化
		// プラットフォームIDはシステム固有で外部から与えられる
		void						create(int platform_idx);
		// OpenCLプラットフォーム解放
		void						release();

		// プロパティ
	public:
		// OpenCLプラットフォームID
		cl_platform_id				getPlatformId() const;
		// OpenCLコンテキスト（複数のデバイスを含む）
		cl_context					getContext() const;
		// デバイス数
		int							numDevices() const;
		// デバイスを取得（GPU１ボード）
		Device&						getDevice(int idx) const;
		// すべてのデバイスを取得
		std::vector<Device*>		getAllDevices() const;
		// コピー禁止・ムーブ禁止

	public:
		DeviceContext(const DeviceContext&) = delete;
		DeviceContext(DeviceContext&&) = delete;
		DeviceContext& operator=(const DeviceContext&) = delete;
		DeviceContext& operator=(DeviceContext&&) = delete;
	};

} // namespace monju

#endif // _MONJU_DEVICE_CONTEXT_H__