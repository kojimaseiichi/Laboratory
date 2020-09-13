#pragma once
#ifndef _MONJU_CL_EVENT_JOINER_H__
#define _MONJU_CL_EVENT_JOINER_H__

#include <vector>
#include <memory>
#include <CL/cl.h>
#include "Synchronizable.h"

namespace monju
{
	/* forward decleration */
	class _ClEvent;
	class ClEventJoiner;

	class ClEventJoiner : public Synchronizable
	{
	private:
		std::vector<std::unique_ptr<_ClEvent>> _events;

	public:
		ClEventJoiner();
		~ClEventJoiner();

	public:
		void push(cl_event ev);
		void join();
		void merge(ClEventJoiner& obj);

		// コピー禁止・ムーブ禁止
	public:
		ClEventJoiner(const ClEventJoiner&) = delete;
		ClEventJoiner(ClEventJoiner&&) = delete;
		ClEventJoiner& operator=(const ClEventJoiner&) = delete;
		ClEventJoiner& operator=(ClEventJoiner&&) = delete;
	};
}

#endif // !_MONJU_CL_EVENT_JOINER_H__
