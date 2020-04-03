#include "DriverBase.h"

#include "DeviceContext.h"
#include "Device.h"

monju::DriverBase::DriverBase()
{
	_p_context = nullptr;
	_p_device = nullptr;
}

monju::DriverBase::DriverBase(DeviceContext & context, Device & device)
{
	_p_context = &context;
	_p_device = &device;
}


monju::DriverBase::~DriverBase()
{
}

void monju::DriverBase::_create(DeviceContext & dc, Device & device)
{
	_p_context = &dc;
	_p_device = &device;
}

monju::DeviceContext & monju::DriverBase::context()
{
	return *_p_context;
}

monju::Device & monju::DriverBase::device()
{
	return *_p_device;
}

void monju::DriverBase::release()
{
	_p_context = nullptr;
	_p_device = nullptr;
}
