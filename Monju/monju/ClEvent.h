#pragma once
#ifndef _MONJU_CL_EVENT_H__
#define _MONJU_CL_EVENT_H__

#include <CL/cl.h>
#include <vector>
#include "Synchronizable.h"
#include "OpenClException.h"

namespace monju
{
	class ClEvent : public Synchronizable
	{
	private:
		std::vector<cl_event> _events;
		
	public:
		ClEvent()
		{

		}
		~ClEvent()
		{

		}

		void push(cl_event event)
		{
			WriteGuard(this);
			_events.push_back(event);
		}
		void waitAll()
		{
			ReadGuard(this);
			std::vector<cl_event> e;
			{
				WriteGuard(this);
				e.swap(_events);
			}
			cl_int error = clWaitForEvents(static_cast<cl_int>(e.size), e.data());
			if (error != CL_SUCCESS)
				throw OpenClException(error, "clWaitForEvents");
		}
	};
}

#endif // !_MONJU_CL_EVENT_H__
