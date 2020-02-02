#pragma once
#ifndef _MONJU_CL_PLATFORM_ID_H__
#define _MONJU_CL_PLATFORM_ID_H__

#include <CL/cl.h>
#include <vector>
#include "OpenClException.h"

namespace monju {

	class ClPlatformId
	{
	private:

		std::vector<cl_platform_id> _platformIds;
		std::vector<cl_device_id> _deviceIds;
		size_t selected;

		void _getPlatformIds()
		{
			cl_platform_id platforms[100];
			cl_uint retNum;
			cl_int error = clGetPlatformIDs(100, platforms, &retNum);
			if (error != CL_SUCCESS)
				throw OpenClException(error, "clGetPlatformIDs");

			_platformIds.clear();
			for (int n = 0; n < retNum; n++)
				_platformIds.push_back(platforms[n]);
		}
		void _getDeviceIds(cl_platform_id platform)
		{
			cl_device_id devices[100];
			cl_uint retNum;
			cl_int error = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 100, devices, &retNum);
			if (error != CL_SUCCESS)
				throw OpenClException(error, "clGetDeviceIDs");
			for (int n = 0; n < retNum; n++)
				_deviceIds.push_back
				(devices[n]);
		}


	public:
		ClPlatformId(int selectPlatform)
		{
			_getPlatformIds();
			if (selectPlatform < 0 || selectPlatform >= _platformIds.size())
				throw MonjuException("selectPlatformが範囲外");
			selected = static_cast<size_t>(selectPlatform);

			_getDeviceIds(_platformIds.at(selected));
		}
		~ClPlatformId();
		std::vector<cl_platform_id> allPlatforms() const
		{
			return _platformIds;
		}
		cl_platform_id platform() const
		{
			return _platformIds.at(selected);
		}
		std::vector<cl_device_id> deviceIds() const
		{
			return _deviceIds;
		}

		// コピー禁止・ムーブ禁止
	public:
		ClPlatformId(const ClPlatformId&) = delete;
		ClPlatformId(ClPlatformId&&) = delete;
		ClPlatformId& operator=(const ClPlatformId&) = delete;
		ClPlatformId& operator=(ClPlatformId&&) = delete;
	};

}

#endif // !_MONJU_CL_PLATFORM_ID_H__
