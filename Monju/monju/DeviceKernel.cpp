#include "DeviceKernel.h"
#include "MonjuException.h" 
#include <fstream>
#include <map>
#include <boost/regex.hpp>

monju::DeviceKernel::~DeviceKernel()
{
	release();
}


void monju::DeviceKernel::compute(std::vector<size_t>& global_work_size)
{
	_run(
		(cl_int)global_work_size.size(),
		global_work_size.data(),
		nullptr
	);
}

