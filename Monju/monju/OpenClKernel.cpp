#include "OpenClKernel.h"

void monju::OpenClKernel::_createProgram()
{
	_clProgram = std::make_shared<ClProgram>(
		_pClContext->context(),
		_pClContext->deviceIds(),
		_sourcePath,
		_params);
}

void monju::OpenClKernel::_createKernel()
{
	_clKernel = std::make_shared<ClKernel>(
		_clProgram->program(),
		_kernelName,
		_params);
}

void monju::OpenClKernel::_releaseProgram()
{

	bool unique = _clProgram.unique();
	_clProgram.reset();
	if (!unique)
		throw MonjuException("_clProgram");
}

void monju::OpenClKernel::_releaseKernel()
{
	bool unique = _clKernel.unique();
	_clKernel.reset();
	if (!unique)
		throw MonjuException("_clKernel");
}

monju::OpenClKernel::OpenClKernel(ClContext& clContext, std::string sourcePath, std::string kernelName, std::map<std::string, std::string> params)
{
	_createProgram();
	_createKernel();
}

monju::OpenClKernel::~OpenClKernel()
{
	_releaseKernel();
	_releaseProgram();
}

std::weak_ptr<monju::ClProgram> monju::OpenClKernel::clProgram() const
{
	return _clProgram;
}

std::weak_ptr<monju::ClKernel> monju::OpenClKernel::clKernel() const
{
	return _clKernel;
}
