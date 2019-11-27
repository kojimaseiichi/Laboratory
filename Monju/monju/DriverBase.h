#pragma once
#ifndef _MONJU_DRIVE_BASE_H__
#define _MONJU_DRIVE_BASE_H__

#include <stdint.h>
#include "GpuDeviceContext.h"
#include "Device.h"

namespace monju {

	class DriverBase
	{
	protected:
		GpuDeviceContext*	_p_context;
		Device*				_p_device;

	protected:
		DriverBase();
		DriverBase(GpuDeviceContext& context, Device& device);
		virtual ~DriverBase();

		void _create(GpuDeviceContext& dc, Device& device);
	public:
		GpuDeviceContext& context();
		Device& device();

	public:
		virtual void release();
	};
}

#endif // _MONJU_DRIVE_BASE_H__