#pragma once
#ifndef _MONJU_DRIVE_BASE_H__
#define _MONJU_DRIVE_BASE_H__

#include <stdint.h>
#include "DeviceContext.h"
#include "Device.h"

namespace monju {

	class DriverBase
	{
	protected:
		DeviceContext*	_p_context;
		Device*				_p_device;

	protected:
		DriverBase();
		DriverBase(DeviceContext& context, Device& device);
		virtual ~DriverBase();

		void _create(DeviceContext& dc, Device& device);
	public:
		DeviceContext& context();
		Device& device();

	public:
		virtual void release();
	};
}

#endif // _MONJU_DRIVE_BASE_H__