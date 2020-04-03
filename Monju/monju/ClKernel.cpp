#include "ClKernel.h"

#include "MonjuException.h"

#include "_ClProgram.h"
#include "_ClKernel.h"
#include "_ClCommandQueue.h"
#include "_ClContext.h"

#include "ClMachine.h"

void monju::ClKernel::_createProgram()
{
	_clProgram = std::make_shared<_ClProgram>(
		_clContext->context(),
		_clContext->deviceIds(),
		_sourcePath,
		_params);
}

void monju::ClKernel::_createKernel()
{
	_clKernel = std::make_shared<_ClKernel>(
		_clProgram->clProgram(),
		_kernelName,
		_params);
}

void monju::ClKernel::_releaseProgram()
{

	bool unique = _clProgram.use_count() == 1;
	_clProgram.reset();
	if (!unique)
		throw MonjuException("_clProgram");
}

void monju::ClKernel::_releaseKernel()
{
	bool unique = _clKernel.use_count() == 1;
	_clKernel.reset();
	if (!unique)
		throw MonjuException("_clKernel");
}

monju::ClKernel::ClKernel(std::weak_ptr<monju::ClMachine> clMachine, std::string sourcePath, std::string kernelName, param_map params)
{
	_clMachine = clMachine.lock();
	_clContext = _clMachine->clContext().lock();
	_sourcePath = sourcePath;
	_kernelName = kernelName;
	_params = params;

	_createProgram();
	_createKernel();
}

monju::ClKernel::ClKernel(std::weak_ptr<monju::ClMachine> clMachine, std::filesystem::path sourcePath, std::string kernelName, param_map params)
{
	_clMachine = clMachine.lock();
	_clContext = _clMachine->clContext().lock();
	_sourcePath = sourcePath.string();
	_kernelName = kernelName;
	_params = params;

	_createProgram();
	_createKernel();
}

monju::ClKernel::~ClKernel()
{
	_releaseKernel();
	_releaseProgram();
}

cl_program monju::ClKernel::clProgram() const
{
	return _clProgram->clProgram();
}

cl_kernel monju::ClKernel::clKernel() const
{
	return _clKernel->clKernel();
}
