#include "_ClEvent.h"
#include "OpenClException.h"
#include <vector>

monju::_ClEvent::_ClEvent(cl_event ev)
{
	_event = ev;
}

monju::_ClEvent::~_ClEvent()
{
	cl_int error = clReleaseEvent(_event);
	if (error != CL_SUCCESS)
		throw OpenClException(error, "clReleaseEvent");
}

cl_event monju::_ClEvent::clEvent() const
{
	return _event;
}
