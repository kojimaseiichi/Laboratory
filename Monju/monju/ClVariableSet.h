#pragma once
#ifndef _VARIABLE_SET_H__
#define _VARIABLE_SET_H__

#include "MonjuTypes.h"
#include "ClMemory.h"

namespace monju
{
	class ClVariableSet
	{
	private:
		std::map<VariableKind, std::tuple<std::function<void* ()>, std::weak_ptr<ClMemory>>> _variableMap;

	private:
		void _enqueueRead(std::tuple<std::function<void* ()>, std::weak_ptr<ClMemory>>& t, ClDeviceContext& deviceContext, ClEventJoiner* pEvent);
		void _enqueueWrite(std::tuple<std::function<void* ()>, std::weak_ptr<ClMemory>>& t, ClDeviceContext& deviceContext, ClEventJoiner* pEvent);
		template <typename T>
		std::function<void* ()> _makeDataAccessor(std::weak_ptr<T> ptr);

	public:
		template <typename T>
		void add(std::weak_ptr<T> weakPtr, VariableKind variableKind, std::weak_ptr<ClMemory> clMemory);
		void clear();
		void enqueueReadAll(std::weak_ptr<ClDeviceContext> deviceContext, std::weak_ptr<ClEventJoiner> event);
		void enqueueWriteAll(std::weak_ptr<ClDeviceContext> deviceContext, std::weak_ptr<ClEventJoiner> event);
		void enqueueRead(std::weak_ptr<ClDeviceContext> deviceContext, std::weak_ptr<ClEventJoiner> event, VariableKind variable);
		void enqueueWrite(std::weak_ptr<ClDeviceContext> deviceContext, std::weak_ptr<ClEventJoiner> event, VariableKind variable);
		void enqueueRead(std::weak_ptr<ClDeviceContext> deviceContext, std::weak_ptr<ClEventJoiner> event, std::vector<VariableKind> variables);
		void enqueueWrite(std::weak_ptr<ClDeviceContext> deviceContext, std::weak_ptr<ClEventJoiner> event, std::vector<VariableKind> variables);
		std::weak_ptr<ClMemory> getClMemory(VariableKind variableKind);

	};
	template<typename T>
	std::function<void* ()> ClVariableSet::_makeDataAccessor(std::weak_ptr<T> ptr)
	{
		return [ptr]() {
			auto p = ptr.lock();
			return p->data();
		};
	}
	template<typename T>
	void ClVariableSet::add(std::weak_ptr<T> weakPtr, VariableKind variableKind, std::weak_ptr<ClMemory> clMemory)
	{
		auto func = _makeDataAccessor(weakPtr);
		_variableMap.emplace(variableKind, std::make_tuple(func, clMemory));
	}
}

#endif // !_VARIABLE_SET_H__
