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
		DeviceContext()
		{
			_context = nullptr;
			_platform_id = nullptr;;
		}
		~DeviceContext()
		{
			release();
		}

		// コピー禁止・ムーブ禁止
	private:
		DeviceContext(const DeviceContext&) = delete;
		DeviceContext(DeviceContext&&) = delete;
		DeviceContext& operator=(const DeviceContext&) = delete;
		DeviceContext& operator=(DeviceContext&&) = delete;

	private:
		cl_platform_id				_findPlatform(int platform_idx)
		{
			cl_uint num_platforms = 0;
			cl_platform_id platforms_buff[2];
			cl_int error_code = clGetPlatformIDs(2, platforms_buff, &num_platforms);
			if (error_code != CL_SUCCESS)
				throw OpenClException(error_code);
			if (platform_idx >= static_cast<int>(num_platforms))
				throw MonjuException();
			// プラットフォーム選択
			return platforms_buff[platform_idx];
		}
		std::vector<cl_device_id>	_listDevices(cl_platform_id platform_id);
		void						_releaseDevices(std::vector<cl_device_id>* p_device_id_vec)
		{
			// デバイスの開放にclReleaseDeviceを使用してはいけない！！
			// このAPIはサブデバイスの開放で使う
			p_device_id_vec->clear();
		}
		cl_context					_createContext(cl_platform_id platform_id, std::vector<cl_device_id>& device_id_vec);
		std::vector<Device*>		_allocDeviceVec(cl_context context, std::vector<cl_device_id> device_id_vec);
		void						_freeDeviceVec(std::vector<Device*>* p_device_vec)
		{
			for (Device* p : *p_device_vec)
				p->release();
			p_device_vec->clear();
		}


	public:
		// OpenCLプラットフォーム初期化
		// プラットフォームIDはシステム固有で外部から与えられる
		void						create(int platform_idx)
		{
			// idからインスタンスを生成
			_platform_id = _findPlatform(platform_idx);
			_device_id_vec = _listDevices(_platform_id);
			_context = _createContext(_platform_id, _device_id_vec);
			_device_vec = _allocDeviceVec(_context, _device_id_vec);
		}
		// OpenCLプラットフォーム解放
		void						release()
		{
			_freeDeviceVec(&_device_vec);
			if (_context != nullptr)
			{
				cl_int error_code = clReleaseContext(_context);
				if (error_code != CL_SUCCESS)
					throw OpenClException(error_code);
				_context = nullptr;
			}
			_device_id_vec.clear();
			_releaseDevices(&_device_id_vec);
			_platform_id = nullptr;
		}

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
	};

} // namespace monju

#endif // _MONJU_DEVICE_CONTEXT_H__