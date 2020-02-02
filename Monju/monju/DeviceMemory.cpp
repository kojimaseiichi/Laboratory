#include "DeviceMemory.h"
#include "util_math.h"

monju::DeviceMemory::DeviceMemory(Device& device) :
	_read_required(),
	_write_required()
{
	_p_device = &device;
}

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

void monju::DeviceMemory::_writeBuffer(Device& device, VariableKind kind)
{
	MemAttr ma;
	if (_findClMem(kind, &ma) == false)
		return;
	_queueWriteBuffer(device.getClCommandQueue(), ma.mem, ma.bytes, ma.p);
}

void monju::DeviceMemory::_readBuffer(Device& device, VariableKind kind)
{
	MemAttr ma;
	if (_findClMem(kind, &ma) == false)
		return;
	_queueReadBuffer(device.getClCommandQueue(), ma.mem, ma.bytes, ma.p);
}

void monju::DeviceMemory::release()
{
	_clearAllClMem();
	_map_mem.clear();
}

void monju::DeviceMemory::writeBuffer(Device& device, VariableKind kind)
{
	_writeBuffer(device, kind);
	_write_required.erase(kind);
}

// ホストメモリからデバイスメモリへ書き込み（デバイス指定）

void monju::DeviceMemory::writeBuffer(Device& device, std::set<VariableKind>& variableKindSet)
{
	for (const auto kind : variableKindSet)
	{
		_writeBuffer(device, kind);
		if (_p_device->getClDeviceId() == device.getClDeviceId())
		{
			_write_required.erase(kind);
		}
	}
}

// ホストメモリからデバイスメモリへ書き込み

void monju::DeviceMemory::writeBuffer(std::set<VariableKind>& variableKindSet)
{
	for (const auto& kind : variableKindSet)
	{
		_writeBuffer(*_p_device, kind);
		_write_required.erase(kind);

	}
}

void monju::DeviceMemory::writeBuffer(VariableKind kind)
{
	_writeBuffer(*_p_device, kind);
	_write_required.erase(kind);
}

void monju::DeviceMemory::readBuffer(Device& device, VariableKind kind)
{
	_readBuffer(device, kind);
	if (_p_device->getClDeviceId() == device.getClDeviceId())
		_read_required.erase(kind);
}

// デバイスメモリからホストメモリへ読み込み（デバイス指定）

void monju::DeviceMemory::readBuffer(Device& device, std::set<VariableKind>& variableKindSet)
{
	for (const auto kind : variableKindSet)
		_readBuffer(device, kind);
	if (_p_device == &device)
	{
		for (auto kind : variableKindSet)
		{
			if (_read_required.find(kind) != _read_required.end())
				_read_required.erase(kind);
		}
	}
}

// デバイスメモリからホストメモリへ読み込み

void monju::DeviceMemory::readBuffer(std::set<VariableKind>& variableKindSet)
{
	for (const auto kind : variableKindSet)
	{
		_readBuffer(*_p_device, kind);
		_read_required.erase(kind);
	}
}

void monju::DeviceMemory::readBuffer(VariableKind kind)
{
	_readBuffer(*_p_device, kind);
	_read_required.erase(kind);
}

void monju::DeviceMemory::requireRead(VariableKind v)
{
	_read_required.insert(v);
}

void monju::DeviceMemory::requireRead(std::set<VariableKind>& variablesToRead)
{
	for (auto const e : variablesToRead)
		requireRead(e);
}

void monju::DeviceMemory::requireWrite(VariableKind v)
{
	_write_required.insert(v);
}

void monju::DeviceMemory::requireWrite(std::set<VariableKind>& variablesToWrite)
{
	for (auto const e : variablesToWrite)
		requireWrite(e);
}

void monju::DeviceMemory::flushWrite()
{
	if (_checkReadWriteConflict() == false)
		throw MonjuException("read/write conflict");
	std::set<VariableKind> c(_write_required);
	writeBuffer(c);
}

void monju::DeviceMemory::flushRead()
{
	if (_checkReadWriteConflict() == false)
		throw MonjuException("read/write conflict");
	std::set<VariableKind> c(_read_required);
	readBuffer(c);
}

// 指定した変数のメモリオブジェクトを取得

cl_mem monju::DeviceMemory::getMemory(VariableKind v)
{
	auto it = _map_mem.find(v);
	if (it != _map_mem.end())
		return it->second.mem;
	return nullptr;
}

