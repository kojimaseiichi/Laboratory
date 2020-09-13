#include "ClVariableSet.h"

#include "VariableKind.h"
#include "ClMemory.h"
#include "ClDeviceContext.h"
#include "ClEventJoiner.h"


void monju::ClVariableSet::_enqueue_read(std::tuple<std::function<void* ()>, std::weak_ptr<ClMemory>>& t, ClDeviceContext& deviceContext, ClEventJoiner* pEvent)
{
	auto [func, clMemory] = t;
	void* pData = func();
	deviceContext.enqueue_read_buffer(clMemory, pData, pEvent);
}

void monju::ClVariableSet::_enqueue_write(std::tuple<std::function<void* ()>, std::weak_ptr<ClMemory>>& t, ClDeviceContext& deviceContext, ClEventJoiner* pEvent)
{
	auto [func, clMemory] = t;
	void* pData = func();
	deviceContext.enqueue_write_buffer(clMemory, pData, pEvent);
}

void monju::ClVariableSet::clear()
{
	_variableMap.clear();
}

void monju::ClVariableSet::enqueue_read_all(std::weak_ptr<ClDeviceContext> deviceContext, std::weak_ptr<ClEventJoiner> event)
{
	auto dc = deviceContext.lock();
	auto ev = event.lock();
	for (auto e : _variableMap)
		_enqueue_read(e.second, *dc, ev.get());
}

void monju::ClVariableSet::enqueue_write_all(std::weak_ptr<ClDeviceContext> deviceContext, std::weak_ptr<ClEventJoiner> event)
{
	auto p = deviceContext.lock();
	auto ev = event.lock();
	for (auto e : _variableMap)
		_enqueue_write(e.second, *p, ev.get());
}

void monju::ClVariableSet::enqueue_read(std::weak_ptr<ClDeviceContext> deviceContext, std::weak_ptr<ClEventJoiner> event, VariableKind variable)
{
	auto p = deviceContext.lock();
	auto ev = event.lock();
	_enqueue_read(_variableMap.at(variable), *p, ev.get());
}

void monju::ClVariableSet::enqueue_write(std::weak_ptr<ClDeviceContext> deviceContext, std::weak_ptr<ClEventJoiner> event, VariableKind variable)
{
	auto p = deviceContext.lock();
	auto ev = event.lock();
	_enqueue_write(_variableMap.at(variable), *p, ev.get());
}

void monju::ClVariableSet::enqueue_read(std::weak_ptr<ClDeviceContext> deviceContext, std::weak_ptr<ClEventJoiner> event, std::vector<VariableKind> variables)
{
	auto p = deviceContext.lock();
	auto ev = event.lock();
	for (auto va : variables)
		_enqueue_read(_variableMap.at(va), *p, ev.get());
}

void monju::ClVariableSet::enqueue_write(std::weak_ptr<ClDeviceContext> deviceContext, std::weak_ptr<ClEventJoiner> event, std::vector<VariableKind> variables)
{
	auto p = deviceContext.lock();
	auto ev = event.lock();
	for (auto va : variables)
		_enqueue_write(_variableMap.at(va), *p, ev.get());
}

std::weak_ptr<monju::ClMemory> monju::ClVariableSet::get_cl_memory(VariableKind variableKind)
{
	auto [func, clMemory] = _variableMap.at(variableKind);
	return clMemory;
}
