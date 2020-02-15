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
	public:
		template <typename M>
		void add(std::function<void* (M&)>&& dataGetter, std::weak_ptr<M> matrix, VariableKind variableKind, std::weak_ptr<ClMemory> clMemory)
		{
			std::function<void* (std::weak_ptr<M>)> wrapper = [](m) ->{
				auto p = m.lock();
				return dataGetter(*p);
			};
			std::function<void*()> func = std::bind(
				std::forward<std::function<void* (std::weak_ptr<M>)>>(wrapper),
				std::forward<std::weak_ptr<M>>(matrix));
			_dataGetterMap.insert(variableKind, std::make_tuple(func, clMemory);
		}
		void clear()
		{
			_variableMap.clear();
		}
		void enqueueReadAll(ClDeviceContext& deviceContext, ClEventJoiner* pEvent)
		{
			for (auto e : _variableMap)
				_enqueueRead(e.second, deviceContext, pEvent);
		}
		void enqueueWriteAll(ClDeviceContext& deviceContext, ClEventJoiner* pEvent)
		{
			for (auto e : _variableMap)
				_enqueueWrite(e.second, deviceContext, pEvent);
		}
		void enqueueRead(ClDeviceContext& deviceContext, ClEventJoiner* pEvent, VariableKind variable)
		{
			_enqueueRead(_variableMap.at(variable), deviceContext, pEvent);
		}
		void enqueueWrite(ClDeviceContext& deviceContext, ClEventJoiner* pEvent, VariableKind variable)
		{
			_enqueueWrite(_variableMap.at(variable), deviceContext, pEvent);
		}
		void enqueueRead(ClDeviceContext& deviceContext, ClEventJoiner* pEvent, std::vector<VariableKind> variables)
		{
			for (auto va : variables)
				enqueueRead(deviceContext, pEvent, va);
		}
		void enqueueWrite(ClDeviceContext& deviceContext, ClEventJoiner* pEvent, std::vector<VariableKind> variables)
		{
			for (auto va : variables)
				enqueueWrite(deviceContext, pEvent, va);
		}
		std::weak_ptr<ClMemory> getClMemory(VariableKind variableKind)
		{
			auto [func, clMemory] = _variableMap.at(variableKind);
			return clMemory;
		}

	};
}

#endif // !_VARIABLE_QUEUE_H__
