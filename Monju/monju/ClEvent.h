#pragma once
#ifndef _MONJU_CL_EVENT_H__
#define _MONJU_CL_EVENT_H__

#include <CL/cl.h>
#include <vector>

namespace monju
{
	class ClEvent
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
			_events.push_back(event);
		}
		void clear()
		{
			_events.clear();
		}
		void clear(std::vector<cl_event> events)
		{
		}
		std::vector<cl_event> events()
		{
			return _events;
		}
	};
}

#endif // !_MONJU_CL_EVENT_H__
