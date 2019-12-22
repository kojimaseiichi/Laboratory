#pragma once
#ifndef _MONJU_DEVICE_CONTEXT_H__
#define _MONJU_DEVICE_CONTEXT_H__

#include <vector>
#include <atomic>
#include <iostream>
#include <CL/cl.h>
#include <crtdbg.h>
#include "MonjuException.h"
#include "Device.h"

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

		// コピー禁止
	private:
		DeviceContext(const DeviceContext& o) = delete;
		DeviceContext& operator=(const DeviceContext& o) = delete;

	private:
		// プラットフォームをヒューリスティックに取得したいが、方法が思いつかないので最初のプラットフォームを取得
		cl_platform_id				_findPlatform(int platform_idx);
		// デバイスをヒューリスティックに取得したいが、方法が思いつかないので最初のデバイスを取得
		// GPU１個であれば問題ないが、複数のGPUを搭載する場合は複数のデバイスを取得するようにする。
		std::vector<cl_device_id>	_listDevices(cl_platform_id platform_id);
		void						_releaseDevices(std::vector<cl_device_id>* p_device_id_vec);
		cl_context					_createContext(cl_platform_id platform_id, std::vector<cl_device_id>& device_id_vec);
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
	};

} // namespace monju

#endif // _MONJU_DEVICE_CONTEXT_H__