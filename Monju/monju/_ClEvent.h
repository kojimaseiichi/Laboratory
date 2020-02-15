#pragma once
#ifndef _MONJU__CL_EVENT_H__
#define _MONJU__CL_EVENT_H__

#include <CL/cl.h>
#include <vector>
#include "Synchronizable.h"
#include "OpenClException.h"

namespace monju
{
	class _ClEvent : public Synchronizable
	{
	private:
		cl_event _event;
		
	public:
		_ClEvent(cl_event ev)
		{
			_event = ev;
		}
		~_ClEvent()
		{
			cl_int error = clReleaseEvent(_event);
			if (error != CL_SUCCESS)
				throw OpenClException(error, "clReleaseEvent");
		}
		cl_event clEvent() const
		{
			return _event;
		}

		//void push(cl_event event)
		//{
		//	WriteGuard write(this);
		//	_events.push_back(event);
		//}
		//void waitAll()
		//{
		//	ReadGuard read(this);
		//	std::vector<cl_event> e;
		//	{
		//		WriteGuard write(this);
		//		e.swap(_events);
		//	}
		//	cl_int error = clWaitForEvents(static_cast<cl_int>(e.size()), e.data());
		//	if (error != CL_SUCCESS)
		//		throw OpenClException(error, "clWaitForEvents");
		//}

				// コピー禁止・ムーブ禁止
	public:
		_ClEvent(const _ClEvent&) = delete;
		_ClEvent(_ClEvent&&) = delete;
		_ClEvent& operator=(const _ClEvent&) = delete;
		_ClEvent& operator=(_ClEvent&&) = delete;

	};
}

#endif // !_MONJU__CL_EVENT_H__
