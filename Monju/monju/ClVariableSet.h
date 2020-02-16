#pragma once
#ifndef _VARIABLE_QUEUE_H__
#define _VARIABLE_QUEUE_H__

#include <functional>
#include <map>
#include <tuple>
#include <vector>
#include "VariableKind.h"
#include "ClMemory.h"
#include "ClDeviceContext.h"
#include "ClEventJoiner.h"

namespace monju
{
	class ClVariableSet
	{
	private:
		std::map<VariableKind, std::tuple<std::function<void* ()>, std::weak_ptr<ClMemory>>> _variableMap;

	private:
		void _enqueueRead(std::tuple<std::function<void* ()>, std::weak_ptr<ClMemory>>& t, ClDeviceContext& deviceContext, ClEventJoiner* pEvent)
		{
			auto [func, clMemory] = t;
			void* pData = func();
			deviceContext.enqueueReadBuffer(clMemory, pData, pEvent);
		}
		void _enqueueWrite(std::tuple<std::function<void* ()>, std::weak_ptr<ClMemory>>& t, ClDeviceContext& deviceContext, ClEventJoiner* pEvent)
		{
			auto [func, clMemory] = t;
			void* pData = func();
			deviceContext.enqueueWriteBuffer(clMemory, pData, pEvent);
		}
		template <typename T>
		std::function<void* ()> _makeDataAccessor(std::weak_ptr<T> ptr)
		{
			return [ptr]() {
				auto p = ptr.lock();
				return p->data();
			};
		}

	public:
		template <typename T>
		void add(std::weak_ptr<T> weakPtr, VariableKind variableKind, std::weak_ptr<ClMemory> clMemory)
		{
			auto func = _makeDataAccessor(weakPtr);
			_variableMap.emplace(variableKind, std::make_tuple(func, clMemory));
		}
		void clear()
		{
			_variableMap.clear();
		}
		void enqueueReadAll(std::weak_ptr<ClDeviceContext> deviceContext, ClEventJoiner* pEvent)
		{
			auto p = deviceContext.lock();
			for (auto e : _variableMap)
				_enqueueRead(e.second, *p, pEvent);
		}
		void enqueueWriteAll(std::weak_ptr<ClDeviceContext> deviceContext, ClEventJoiner* pEvent)
		{
			auto p = deviceContext.lock();
			for (auto e : _variableMap)
				_enqueueWrite(e.second, *p, pEvent);
		}
		void enqueueRead(std::weak_ptr<ClDeviceContext> deviceContext, ClEventJoiner* pEvent, VariableKind variable)
		{
			auto p = deviceContext.lock();
			_enqueueRead(_variableMap.at(variable), *p, pEvent);
		}
		void enqueueWrite(std::weak_ptr<ClDeviceContext> deviceContext, ClEventJoiner* pEvent, VariableKind variable)
		{
			auto p = deviceContext.lock();
			_enqueueWrite(_variableMap.at(variable), *p, pEvent);
		}
		void enqueueRead(std::weak_ptr<ClDeviceContext> deviceContext, ClEventJoiner* pEvent, std::vector<VariableKind> variables)
		{
			auto p = deviceContext.lock();
			for (auto va : variables)
				_enqueueRead(_variableMap.at(va), *p, pEvent);
		}
		void enqueueWrite(std::weak_ptr<ClDeviceContext> deviceContext, ClEventJoiner* pEvent, std::vector<VariableKind> variables)
		{
			auto p = deviceContext.lock();
			for (auto va : variables)
				_enqueueWrite(_variableMap.at(va), *p, pEvent);
		}
		std::weak_ptr<ClMemory> getClMemory(VariableKind variableKind)
		{
			auto [func, clMemory] = _variableMap.at(variableKind);
			return clMemory;
		}

	};
}

#endif // !_VARIABLE_QUEUE_H__
