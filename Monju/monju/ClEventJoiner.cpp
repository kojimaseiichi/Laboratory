#include "ClEventJoiner.h"
#include "_ClEvent.h"
#include "OpenClException.h"


monju::ClEventJoiner::ClEventJoiner()
{

}

monju::ClEventJoiner::~ClEventJoiner()
{
	_events.clear();
}

void monju::ClEventJoiner::push(cl_event ev)
{
	WriteGuard write(this);	// 書き込みロック
	std::unique_ptr<_ClEvent> p = std::make_unique<_ClEvent>(ev);
	_events.push_back(std::move(p));
}

void monju::ClEventJoiner::join()
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

void monju::ClEventJoiner::merge(ClEventJoiner& obj)
{
	WriteGuard write1(this);
	WriteGuard write2(obj);
	for (auto& e : obj._events)
		_events.push_back(std::move(e));
	obj._events.clear();
}
