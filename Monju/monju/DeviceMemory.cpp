#include "DeviceMemory.h"
#include "util_math.h"

monju::DeviceMemory::~DeviceMemory()
{
	release();
}

void monju::DeviceMemory::_clearAllClMem()
{
	for (auto&& e : _map_mem)
	{
		cl_int error_code = clReleaseMemObject(e.second.mem);
		if (error_code != CL_SUCCESS)
			throw OpenClException(error_code);
	}
}

bool monju::DeviceMemory::_findClMem(VariableKind t, MemAttr* out)
{
	auto it = _map_mem.find(t);
	if (it != _map_mem.end())
	{
		*out = it->second;
		return true;
	}
	return false;
}

bool monju::DeviceMemory::_checkReadWriteConflict()
{
	bool conflicted = util_math::has_intersection(_read_required, _write_required);
	if (conflicted)
		return false;
	return true;
}

void monju::DeviceMemory::release()
{
	_clearAllClMem();
	_map_mem.clear();
}

void monju::DeviceMemory::writeBuffer(Device& device, std::set<monju::VariableKind> variableKindSet)
{
	for (const auto& kind : variableKindSet)
	{
		MemAttr ma;
		if (_findClMem(kind, &ma) == false)
			continue;
		_queueWriteBuffer(device.getClCommandQueue(), ma.mem, ma.bytes, ma.p);
	}
}

void monju::DeviceMemory::writeBuffer(std::set<monju::VariableKind> variableKindSet)
{
	writeBuffer(*_p_device, variableKindSet);
	_write_required.erase(variableKindSet.begin(), variableKindSet.end());

}

void monju::DeviceMemory::readBuffer(Device& device, std::set<monju::VariableKind> variableKindSet)
{
	for (const auto& kind : variableKindSet)
	{
		MemAttr ma;
		if (_findClMem(kind, &ma) == false)
			continue;
		_queueReadBuffer(device.getClCommandQueue(), ma.mem, ma.bytes, ma.p);
	}
}

void monju::DeviceMemory::readBuffer(std::set<monju::VariableKind> variableKindSet)
{
	readBuffer(*_p_device, variableKindSet);
	_read_required.erase(variableKindSet.begin(), variableKindSet.end());
}

void monju::DeviceMemory::requireRead(VariableKind v)
{
	_read_required.insert(v);
}

void monju::DeviceMemory::requireRead(std::set<VariableKind> variablesToRead)
{
	for (auto const e : variablesToRead)
		requireRead(e);
}

void monju::DeviceMemory::requireWrite(VariableKind v)
{
	_write_required.insert(v);
}

void monju::DeviceMemory::requireWrite(std::set<VariableKind> variablesToWrite)
{
	for (auto const e : variablesToWrite)
		requireWrite(e);
}

void monju::DeviceMemory::flushWrite()
{
	if (_checkReadWriteConflict() == false)
		throw MonjuException("read/write conflict");
	writeBuffer(_write_required);
}

void monju::DeviceMemory::flushRead()
{
	if (_checkReadWriteConflict() == false)
		throw MonjuException("read/write conflict");
	readBuffer(_read_required);
}

// 指定した変数のメモリオブジェクトを取得

cl_mem monju::DeviceMemory::getMemory(VariableKind v)
{
	auto it = _map_mem.find(v);
	if (it != _map_mem.end())
		return it->second.mem;
	return nullptr;
}

