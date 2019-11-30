#include "DeviceMemory.h"


monju::DeviceMemory::DeviceMemory() :
	_read_required(),
	_write_required()
{
	_p_dc = nullptr;
	_p_device = nullptr;
}


monju::DeviceMemory::~DeviceMemory()
{
	release();
}

void monju::DeviceMemory::_clearAllClMem()
{
	for (auto&& e : _map_mem)
		clReleaseMemObject(e.second.mem);
}

bool monju::DeviceMemory::_findClMem(VariableKind t, MemAttr* out)
{
	auto it = _map_mem.find(t);
	if (it != _map_mem.end())
	{
		out = &(it->second);
		return true;
	}
	return false;
}

bool monju::DeviceMemory::_checkReadWriteConflict()
{
	boost::dynamic_bitset<> conflict = _read_required & _write_required;
	if (conflict.any())
		return false;
	return true;
}

void monju::DeviceMemory::release()
{
	_clearAllClMem();
	_map_mem.clear();
}

inline void monju::DeviceMemory::create(DeviceContext& dc, Device& device)
{
	_p_dc = &dc;
	_p_device = &device;
}

void monju::DeviceMemory::writeBuffer(Device& device, boost::dynamic_bitset<> variableKindSet)
{
	for (size_t i = 0; i < variableKindSet.size(); i++)
	{
		if (variableKindSet.test(i) == false)
			continue;
		MemAttr ma;
		if (_findClMem(static_cast<VariableKind>(i), &ma) == false)
			continue;
		_queueWriteBuffer(device.getCommandQueue(), ma.mem, ma.bytes, ma.p);
	}
}

void monju::DeviceMemory::writeBuffer(boost::dynamic_bitset<> variableKindSet)
{
	writeBuffer(*_p_device, variableKindSet);

}

void monju::DeviceMemory::readBuffer(Device& device, boost::dynamic_bitset<> variableKindSet)
{
	for (size_t i = 0; i < variableKindSet.size(); i++)
	{
		if (variableKindSet.test(i) == false)
			continue;
		MemAttr ma;
		if (_findClMem(static_cast<VariableKind>(i), &ma) == false)
			continue;
		_queueReadBuffer(device.getCommandQueue(), ma.mem, ma.bytes, ma.p);
	}
}

void monju::DeviceMemory::readBuffer(boost::dynamic_bitset<> variableKindSet)
{
	readBuffer(*_p_device, variableKindSet);
	_read_required &= variableKindSet;
}

void monju::DeviceMemory::requireRead(VariableKind v)
{
	boost::dynamic_bitset<> flags(static_cast<uint64_t>(v));
	_read_required |= flags;
}

void monju::DeviceMemory::requireWrite(VariableKind v)
{
	boost::dynamic_bitset<> flags(static_cast<uint64_t>(v));
	_write_required |= flags;
}

void monju::DeviceMemory::flushWrite()
{
	_checkReadWriteConflict();
	writeBuffer(_write_required);
}

void monju::DeviceMemory::flushRead()
{
	_checkReadWriteConflict();
	readBuffer(_read_required);
}

// 指定した変数のメモリオブジェクトを取得

inline cl_mem monju::DeviceMemory::getMemory(VariableKind v)
{
	auto it = _map_mem.find(v);
	if (it != _map_mem.end())
		return it->second.mem;
	return nullptr;
}

