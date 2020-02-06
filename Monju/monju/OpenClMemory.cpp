#include "OpenClMemory.h"

void monju::OpenClMemory::_createBuffer()
{
	auto pc = _clContext.lock();
	_clBuffer = std::make_shared<ClBuffer>(pc->context(), _size, _p);
}

void monju::OpenClMemory::_releaseBuffer()
{
	bool unique = _clBuffer.unique();
	_clBuffer.reset();
	if (!unique)
		throw MonjuException("_clBuffer");
}

monju::OpenClMemory::OpenClMemory(std::weak_ptr<ClContext> clContext, size_t size, void* p)
{
	_clContext = clContext;
	_size = size;
	_p = p;
	_createBuffer();
}

monju::OpenClMemory::~OpenClMemory()
{
	_releaseBuffer();
}

std::weak_ptr<ClBuffer> monju::OpenClMemory::clBuffer() const
{
	return _clBuffer;
}
