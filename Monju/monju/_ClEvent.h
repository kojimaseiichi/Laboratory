#pragma once
#ifndef _MONJU__CL_EVENT_H__
#define _MONJU__CL_EVENT_H__

#include "MonjuTypes.h"
#include "Synchronizable.h"

namespace monju
{
	class _ClEvent : public Synchronizable
	{
	private:
		cl_event _event;
		
	public:
		_ClEvent(cl_event ev);
		~_ClEvent();
		cl_event clEvent() const;

	// コピー禁止・ムーブ禁止
	public:
		_ClEvent(const _ClEvent&) = delete;
		_ClEvent(_ClEvent&&) = delete;
		_ClEvent& operator=(const _ClEvent&) = delete;
		_ClEvent& operator=(_ClEvent&&) = delete;

	};
}

#endif // !_MONJU__CL_EVENT_H__
