#pragma once
#ifndef _MONJU_CL_EVENT_JOINER_H__
#define _MONJU_CL_EVENT_JOINER_H__

#include "_ClEvent.h"
#include "Synchronizable.h"

namespace monju
{
	class ClEventJoiner : public Synchronizable
	{
	private:
		std::vector<std::unique_ptr<_ClEvent>> _events;

	public:
		ClEventJoiner()
		{

		}
		~ClEventJoiner()
		{
			_events.clear();
		}
		void push(cl_event ev)
		{
			WriteGuard write(this);	// 書き込みロック
			std::unique_ptr<_ClEvent> p = std::make_unique<_ClEvent>(ev);
			_events.push_back(std::move(p));
		}
		void join()
		{
			std::vector<std::unique_ptr<_ClEvent>> cp;
			{
				WriteGuard write(this);	// 書き込みロック
				for (auto& e : _events)
					cp.push_back(std::move(e));
				_events.clear();
			}
			std::vector<cl_event> raw;
			for (auto& e : cp)
				raw.push_back(e->clEvent());
			cl_int error = clWaitForEvents(static_cast<cl_int>(raw.size()), raw.data());
			if (error != CL_SUCCESS)
				throw OpenClException(error, "clWaitForEvents");
		}
		void merge(ClEventJoiner& obj)
		{
			WriteGuard write1(this);
			WriteGuard write2(obj);
			for (auto& e : obj._events)
				_events.push_back(std::move(e));
			obj._events.clear();
		}

		// コピー禁止・ムーブ禁止
	public:
		ClEventJoiner(const ClEventJoiner&) = delete;
		ClEventJoiner(ClEventJoiner&&) = delete;
		ClEventJoiner& operator=(const ClEventJoiner&) = delete;
		ClEventJoiner& operator=(ClEventJoiner&&) = delete;
	};
}

#endif // !_MONJU_CL_EVENT_JOINER_H__
