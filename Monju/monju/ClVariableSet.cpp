#include "ClVariableSet.h"

#include "VariableKind.h"
#include "ClMemory.h"
#include "ClDeviceContext.h"
#include "ClEventJoiner.h"


void monju::ClVariableSet::_enqueueRead(std::tuple<std::function<void* ()>, std::weak_ptr<ClMemory>>& t, ClDeviceContext& deviceContext, ClEventJoiner* pEvent)
{
	auto [func, clMemory] = t;
	void* pData = func();
	deviceContext.enqueueReadBuffer(clMemory, pData, pEvent);
}

void monju::ClVariableSet::_enqueueWrite(std::tuple<std::function<void* ()>, std::weak_ptr<ClMemory>>& t, ClDeviceContext& deviceContext, ClEventJoiner* pEvent)
{
	auto [func, clMemory] = t;
	void* pData = func();
	deviceContext.enqueueWriteBuffer(clMemory, pData, pEvent);
}

void monju::ClVariableSet::clear()
{
	_variableMap.clear();
}

void monju::ClVariableSet::enqueueReadAll(std::weak_ptr<ClDeviceContext> deviceContext, ClEventJoiner* pEvent)
{
	auto p = deviceContext.lock();
	for (auto e : _variableMap)
		_enqueueRead(e.second, *p, pEvent);
}

void monju::ClVariableSet::enqueueWriteAll(std::weak_ptr<ClDeviceContext> deviceContext, ClEventJoiner* pEvent)
{
	auto p = deviceContext.lock();
	for (auto e : _variableMap)
		_enqueueWrite(e.second, *p, pEvent);
}

void monju::ClVariableSet::enqueueRead(std::weak_ptr<ClDeviceContext> deviceContext, ClEventJoiner* pEvent, VariableKind variable)
{
	auto p = deviceContext.lock();
	_enqueueRead(_variableMap.at(variable), *p, pEvent);
}

void monju::ClVariableSet::enqueueWrite(std::weak_ptr<ClDeviceContext> deviceContext, ClEventJoiner* pEvent, VariableKind variable)
{
	auto p = deviceContext.lock();
	_enqueueWrite(_variableMap.at(variable), *p, pEvent);
}

void monju::ClVariableSet::enqueueRead(std::weak_ptr<ClDeviceContext> deviceContext, ClEventJoiner* pEvent, std::vector<VariableKind> variables)
{
	auto p = deviceContext.lock();
	for (auto va : variables)
		_enqueueRead(_variableMap.at(va), *p, pEvent);
}

void monju::ClVariableSet::enqueueWrite(std::weak_ptr<ClDeviceContext> deviceContext, ClEventJoiner* pEvent, std::vector<VariableKind> variables)
{
	auto p = deviceContext.lock();
	for (auto va : variables)
		_enqueueWrite(_variableMap.at(va), *p, pEvent);
}

std::weak_ptr<monju::ClMemory> monju::ClVariableSet::getClMemory(VariableKind variableKind)
{
	auto [func, clMemory] = _variableMap.at(variableKind);
	return clMemory;
}
