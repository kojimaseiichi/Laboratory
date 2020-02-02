#include "DeviceKernel.h"
#include "MonjuException.h" 
#include <fstream>
#include <map>
#include <boost/regex.hpp>

monju::DeviceKernel::~DeviceKernel()
{
	release();
}

// �R���p�C���ς݃J�[�l���v���O�������f�o�C�X�ɔz�u���Ď��s�\�ȏ�ԂɑJ��

cl_kernel monju::DeviceKernel::_createKernel(cl_program program, std::string kernel_name, std::map<std::string, std::string>& params)
{
	std::string parameterized_kernel_name = util_str::parameterizePlaceholders(kernel_name, params);
	cl_int error_code;
	cl_kernel kernel = clCreateKernel(program, parameterized_kernel_name.c_str(), &error_code);
	if (error_code != CL_SUCCESS)
		throw OpenClException(error_code);
	return kernel;
}

// �J�[�l�������s�\�ȏ�ԂɑJ�ځi_compileProgram�A_createKernel�j

void monju::DeviceKernel::_initKernel(cl_program program, std::string kernel_name, std::map<std::string, std::string>& params)
{
	_kernel = _createKernel(program, kernel_name, params);
}

// �v���O�����R���p�C��(CL�t�@�C��)�A�J�[�l������

void monju::DeviceKernel::_create(DeviceProgram& program, std::string kernel_name, std::map<std::string, std::string>& params)
{
	_p_program = &program;
	_kernel_name = kernel_name;
	_initKernel(program.program(), kernel_name, params);
}

// �f�o�C�X��ɔz�u�ς݂̃J�[�l���v���O���������s
// �����͎��O�ɐݒ肵�Ă����K�v������

void monju::DeviceKernel::_run(Device& device, std::vector<size_t>& global_work_size, std::vector<size_t>& local_work_size)
{
	if (global_work_size.size() != local_work_size.size())
		throw MonjuException("global_work_size��local_work_size�̎�������v���Ȃ�");
	cl_uint dim = static_cast<cl_uint>(global_work_size.size());
	cl_event eventWait;
	cl_int error_code = clEnqueueNDRangeKernel(
		device.getClCommandQueue(),	// OpenCL�L���[
		_kernel,						// �J�[�l��
		dim,							// 2����
		nullptr,						// global work offset
		global_work_size.data(),		// global work size
		local_work_size.data(),			// local work size
		0,								// number of events in wait list
		nullptr,						// event wait list
		&eventWait);					// event
	if (error_code != CL_SUCCESS)
		throw OpenClException(error_code);
	error_code = clWaitForEvents(1, &eventWait);
	if (error_code != CL_SUCCESS)
		throw OpenClException(error_code);
	error_code = clReleaseEvent(eventWait);
	if (error_code != CL_SUCCESS)
		throw OpenClException(error_code);
}

void monju::DeviceKernel::_run(Device& device, std::vector<size_t>& global_work_size)
{
	cl_uint dim = static_cast<cl_uint>(global_work_size.size());
	cl_event eventWait;
	cl_int error_code = clEnqueueNDRangeKernel(
		device.getClCommandQueue(),	// OpenCL�L���[
		_kernel,						// �J�[�l��
		dim,							// 2����
		nullptr,						// global work offset
		global_work_size.data(),		// global work size
		nullptr,						// local work size(null)
		0,								// number of events in wait list
		nullptr,						// event wait list
		&eventWait);						// event
	if (error_code != CL_SUCCESS)
		throw OpenClException(error_code);
	error_code = clWaitForEvents(1, &eventWait);
	if (error_code != CL_SUCCESS)
		throw OpenClException(error_code);
	error_code = clReleaseEvent(eventWait);
	if (error_code != CL_SUCCESS)
		throw OpenClException(error_code);
}

// OpenCL�J�[�l������

void monju::DeviceKernel::create(DeviceProgram& program, std::string kernel_name, std::map<std::string, std::string>& params)
{
	_create(program, kernel_name, params);
}

// OpenCL�J�[�l�����

void monju::DeviceKernel::release()
{
	if (_kernel != nullptr)
	{
		cl_int error_code = clReleaseKernel(_kernel);
		if (error_code != CL_SUCCESS)
			throw OpenClException(error_code);
		_kernel = nullptr;
	}
	_p_program = nullptr;
	_kernel_name = "";
}

// �J�[�l���v�Z���s

void monju::DeviceKernel::compute(Device& device, std::vector<size_t>& global_work_size)
{
	_run(device, global_work_size);
}

// �J�[�l���v�Z���s

void monju::DeviceKernel::compute(Device& device, std::vector<size_t>& global_work_size, std::vector<size_t>& local_work_size)
{
	_run(device, global_work_size, local_work_size);
}

// �v���O�����R���p�C��(CL�t�@�C��)�A�J�[�l������

