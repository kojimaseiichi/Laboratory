#pragma once
#ifndef _MONJU__CL_PLATFORM_ID_H__
#define _MONJU__CL_PLATFORM_ID_H__

#include <CL/cl.h>
#include <vector>
#include "OpenClException.h"

namespace monju {

	class _ClPlatformId
	{
	private:

		std::vector<cl_platform_id> _platformIds;
		std::vector<cl_device_id> _deviceIds;
		size_t selectedPlatformId;

		void _getPlatformIds();
		void _getDeviceIds(cl_platform_id platform);


	public:
		_ClPlatformId(int selectPlatform);
		~_ClPlatformId();
		std::vector<cl_platform_id> clPlatformIds() const;
		cl_platform_id clPlatformId() const;
		std::vector<cl_device_id> clDeviceIds() const;

		// コピー禁止・ムーブ禁止
	public:
		_ClPlatformId(const _ClPlatformId&) = delete;
		_ClPlatformId(_ClPlatformId&&) = delete;
		_ClPlatformId& operator=(const _ClPlatformId&) = delete;
		_ClPlatformId& operator=(_ClPlatformId&&) = delete;
	};

}

#endif // !_MONJU__CL_PLATFORM_ID_H__
