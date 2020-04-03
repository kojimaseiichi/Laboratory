#include "ClMemory.h"
#include "ClMachine.h"
#include "_ClContext.h"
#include "_ClBuffer.h"
#include "MonjuException.h"

void monju::ClMemory::_createBuffer()
{
	_clBuffer = std::make_shared<_ClBuffer>(
		_clContext->context(),
		_size);
}

void monju::ClMemory::_releaseBuffer()
{
	bool unique = _clBuffer.use_count() == 1;
	_clBuffer.reset();
	if (!unique)
		throw MonjuException("_clBuffer");
}

monju::ClMemory::ClMemory(std::weak_ptr<ClMachine> clMachine, size_t size)
{
	_clMachine = clMachine.lock();
	_clContext = _clMachine->clContext().lock();
	_size = size;
	_createBuffer();
}

monju::ClMemory::~ClMemory()
{
	_releaseBuffer();
}

std::weak_ptr<monju::_ClBuffer> monju::ClMemory::clBuffer() const
{
	return _clBuffer;
}

size_t monju::ClMemory::size() const
{
	return _size;
}
