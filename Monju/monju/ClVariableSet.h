#pragma once
#ifndef _VARIABLE_SET_H__
#define _VARIABLE_SET_H__

#include <memory>
#include <functional>
#include <map>
#include <tuple>

namespace monju
{
	class ClMemory;
	class ClDeviceContext;
	class ClEventJoiner;
	enum class VariableKind;

	class ClVariableSet
	{
	private:
		std::map<VariableKind, std::tuple<std::function<void* ()>, std::weak_ptr<ClMemory>>> _variableMap;

	private:
		void _enqueue_read(std::tuple<std::function<void* ()>, std::weak_ptr<ClMemory>>& t, ClDeviceContext& deviceContext, ClEventJoiner* pEvent);
		void _enqueue_write(std::tuple<std::function<void* ()>, std::weak_ptr<ClMemory>>& t, ClDeviceContext& deviceContext, ClEventJoiner* pEvent);
		template <typename T>
		std::function<void* ()> _make_data_accessor(std::weak_ptr<T> ptr);

	public:
		template <typename T>
		void add(std::weak_ptr<T> weakPtr, VariableKind variableKind, std::weak_ptr<ClMemory> clMemory);
		void clear();
		void enqueue_read_all(std::weak_ptr<ClDeviceContext> deviceContext, std::weak_ptr<ClEventJoiner> event);
		void enqueue_write_all(std::weak_ptr<ClDeviceContext> deviceContext, std::weak_ptr<ClEventJoiner> event);
		void enqueue_read(std::weak_ptr<ClDeviceContext> deviceContext, std::weak_ptr<ClEventJoiner> event, VariableKind variable);
		void enqueue_write(std::weak_ptr<ClDeviceContext> deviceContext, std::weak_ptr<ClEventJoiner> event, VariableKind variable);
		void enqueue_read(std::weak_ptr<ClDeviceContext> deviceContext, std::weak_ptr<ClEventJoiner> event, std::vector<VariableKind> variables);
		void enqueue_write(std::weak_ptr<ClDeviceContext> deviceContext, std::weak_ptr<ClEventJoiner> event, std::vector<VariableKind> variables);
		std::weak_ptr<ClMemory> get_cl_memory(VariableKind variableKind);

	};

	template<typename T>
	std::function<void* ()> ClVariableSet::_make_data_accessor(std::weak_ptr<T> ptr)
	{
		return [ptr]() {
			auto p = ptr.lock();
			return p->data();
		};
	}

	template<typename T>
	void ClVariableSet::add(std::weak_ptr<T> weakPtr, VariableKind variableKind, std::weak_ptr<ClMemory> clMemory)
	{
		auto func = _make_data_accessor(weakPtr);
		_variableMap.emplace(variableKind, std::make_tuple(func, clMemory));
	}
}

#endif // !_VARIABLE_SET_H__
