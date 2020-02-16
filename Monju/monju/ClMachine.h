#pragma once
#ifndef _MONJU_CL_MACHINE_H__
#define _MONJU_CL_MACHINE_H__

#include "_ClPlatformId.h"
#include "_ClContext.h"
#include "_ClCommandQueue.h"
#include <memory>

namespace monju
{
	/*
	platformIdで各マシンのOpenCL実装を選択する。
	*/
	class ClMachine
	{
	private:
		int _platformId;
		std::shared_ptr<_ClPlatformId> _clPlatformId;	// 解放予定
		std::shared_ptr<_ClContext> _clContext;			// 解放予定

		void _createPlatformId();
		void _createContext();
		void _releasePlatformId();
		void _releaseContext();

	public:
		ClMachine(int platformId);
		~ClMachine();
		std::weak_ptr<_ClPlatformId> clPlatformId() const;
		std::weak_ptr<_ClContext> clContext() const;
		std::vector<cl_device_id> deviceIds() const;

		// コピー禁止・ムーブ禁止
	public:
		ClMachine(const ClMachine&) = delete;
		ClMachine(ClMachine&&) = delete;
		ClMachine& operator=(const ClMachine&) = delete;
		ClMachine& operator=(ClMachine&&) = delete;
	};
}

#endif // !_MONJU_CL_MACHINE_H__

