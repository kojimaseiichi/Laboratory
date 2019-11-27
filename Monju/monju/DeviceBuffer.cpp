#include "DeviceBuffer.h"


monju::DeviceBuffer::DeviceBuffer() :
	_read_required(),
	_write_required()
{
	_p_dc = nullptr;
	_p_device = nullptr;
}


monju::DeviceBuffer::~DeviceBuffer()
{
	release();
}

void monju::DeviceBuffer::_clearAllClMem()
{
	for (auto&& e : _map_mem)
		clReleaseMemObject(e.second.mem);
}

bool monju::DeviceBuffer::_findClMem(VariableKind t, MemAttr* out)
{
	auto it = _map_mem.find(t);
	if (it != _map_mem.end())
	{
		out = &(it->second);
		return true;
	}
	return false;
}

bool monju::DeviceBuffer::_checkReadWriteConflict()
{
	boost::dynamic_bitset<> conflict = _read_required & _write_required;
	if (conflict.any())
		return false;
	return true;
}

void monju::DeviceBuffer::release()
{
	_clearAllClMem();
	_map_mem.clear();
}

inline void monju::DeviceBuffer::create(GpuDeviceContext& dc, Device& device)
{
	_p_dc = &dc;
	_p_device = &device;
}

void monju::DeviceBuffer::writeBuffer(Device& device, boost::dynamic_bitset<> variableKindSet)
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

void monju::DeviceBuffer::writeBuffer(boost::dynamic_bitset<> variableKindSet)
{
	writeBuffer(*_p_device, variableKindSet);

}

void monju::DeviceBuffer::readBuffer(Device& device, boost::dynamic_bitset<> variableKindSet)
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

void monju::DeviceBuffer::readBuffer(boost::dynamic_bitset<> variableKindSet)
{
	readBuffer(*_p_device, variableKindSet);
	_read_required &= variableKindSet;
}

cl_mem monju::DeviceBuffer::getClMem(VariableKind v)
{
	auto it = _map_mem.find(v);
	if (it != _map_mem.end())
		return it->second.mem;
	return nullptr;
}

void monju::DeviceBuffer::requireRead(VariableKind v)
{
	boost::dynamic_bitset<> flags(static_cast<uint64_t>(v));
	_read_required |= flags;
}

void monju::DeviceBuffer::requireWrite(VariableKind v)
{
	boost::dynamic_bitset<> flags(static_cast<uint64_t>(v));
	_write_required |= flags;
}

void monju::DeviceBuffer::flushWrite()
{
	_checkReadWriteConflict();
	writeBuffer(_write_required);
}

void monju::DeviceBuffer::flushRead()
{
	_checkReadWriteConflict();
	readBuffer(_read_required);
}

