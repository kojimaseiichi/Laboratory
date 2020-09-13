#pragma once
#ifndef _MONJU_CL_MACHINE_H__
#define _MONJU_CL_MACHINE_H__

#include <memory>
#include <vector>
#include <CL/cl.h>

namespace monju
{
	class _ClPlatformId;
	class _ClContext;

	/*
	platformIdで各マシンのOpenCL実装を選択する。
	*/
	class ClMachine
	{
	private:
		int _platformId;
		std::shared_ptr<_ClPlatformId> _clPlatformId;	// 解放予定
		std::shared_ptr<_ClContext> _clContext;			// 解放予定

	private:
		void _create_platform_id();
		void _create_context();
		void _release_platform_id();
		void _release_context();

	public:
		ClMachine(int platformId);
		~ClMachine();

	public:
		std::weak_ptr<_ClPlatformId> clPlatformId() const;
		std::weak_ptr<_ClContext> clContext() const;
		std::vector<cl_device_id> deviceIds() const;
		cl_device_id defaultDeviceId() const;

		// コピー禁止・ムーブ禁止
	public:
		ClMachine(const ClMachine&) = delete;
		ClMachine(ClMachine&&) = delete;
		ClMachine& operator=(const ClMachine&) = delete;
		ClMachine& operator=(ClMachine&&) = delete;
	};
}

#endif // !_MONJU_CL_MACHINE_H__

