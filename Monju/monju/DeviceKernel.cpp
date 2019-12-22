#include "DeviceKernel.h"
#include "MonjuException.h" 
#include <fstream>
#include <map>
#include <boost/regex.hpp>

monju::DeviceKernel::DeviceKernel()
{
	_program = nullptr;
	_kernel = nullptr;
}


monju::DeviceKernel::~DeviceKernel()
{
	release();
}


// �J�[�l���R���p�C�����ăJ�[�l���v���O�����𐶐��i���̏�Ԃł͎��s�ł��Ȃ��j
// OpenCL�J�[�l���\�[�X��ǂݍ��݁A�e���v���[�g�ϐ��̋�̉��A�J�[�l���R���p�C��

// �R���p�C���ς݃J�[�l���v���O�������f�o�C�X�ɔz�u���Ď��s�\�ȏ�ԂɑJ��


// �J�[�l���R���p�C�����ăJ�[�l���v���O�����𐶐��i���̏�Ԃł͎��s�ł��Ȃ��j
// OpenCL�J�[�l���\�[�X��ǂݍ��݁A�e���v���[�g�ϐ��̋�̉��A�J�[�l���R���p�C��

cl_program monju::DeviceKernel::_compileProgram(cl_context context, cl_device_id device_id, std::string file_path, params_map& params)
{
	std::string plain_source = _getSource(file_path);
	std::string edited_source = _parameterize(plain_source, params);

	const char* p_source = edited_source.c_str();
	const size_t source_size = edited_source.size();

	// �v���O�����̃R���p�C��
	cl_int error_code;
	cl_program program = clCreateProgramWithSource(context, 1, (const char**)&p_source, (const size_t*)&source_size, &error_code);
	if (error_code != CL_SUCCESS)
		throw OpenClException(error_code);
	error_code = clBuildProgram(program, 1, &device_id, nullptr, nullptr, nullptr);
	// �R���p�C���Ɏ��s�����ꍇ�̓G���[���e��\��
	if (error_code != CL_SUCCESS)
	{
		size_t logSize;
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize);
		char* buildLog = (char*)malloc((logSize + 1));
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, logSize, buildLog, nullptr);
		std::string message = buildLog == nullptr ? "" : buildLog;
		free(buildLog);
		throw MonjuException(message);
	}
	return program;
}

cl_kernel monju::DeviceKernel::_createKernel(cl_program program, std::string kernel_name, params_map& params)
{
	std::string parameterized_kernel_name = _parameterize(kernel_name, params);
	cl_kernel kernel = clCreateKernel(program, parameterized_kernel_name.c_str(), NULL);
	if (kernel == nullptr)
		throw MonjuException("�J�[�l���̍쐬�Ɏ��s");
	return kernel;
}

// �J�[�l�������s�\�ȏ�ԂɑJ�ځi_compileProgram�A_createKernel�j

void monju::DeviceKernel::_initKernel(cl_context context, cl_device_id device_id, std::string source_path, std::string kernel_name, params_map& params)
{
	_program = _compileProgram(context, device_id, source_path, params);
	_kernel = _createKernel(_program, kernel_name, params);
}

// �J�[�l�������s�\�ȏ�ԂɑJ�ځi_compileProgram�A_createKernel�j

// �J�[�l���\�[�X�̓��e�𕶎���Ŏ擾

std::string monju::DeviceKernel::_getSource(std::string souce_path)
{
	std::ifstream input(souce_path, std::ifstream::in | std::ifstream::binary);
	std::string cl_source_original((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
	return cl_source_original;
}

// �J�[�l���\�[�X���̃e���v���[�g�ϐ�����̉�

std::string monju::DeviceKernel::_parameterize(std::string source, params_map& params)
{
	auto callback = [&params](boost::match_results<std::string::const_iterator> const& m)->std::string {
		std::string key = m[1];
		auto itr = params.find(key);
		if (itr != params.end())
			return itr->second;
		return "";
	};
	boost::basic_regex<char> reg(R"(\$\{([a-zA-Z_]+)\})");
	return boost::regex_replace(source, reg, callback);

}

// �v���O�����R���p�C��(CL�t�@�C��)�A�J�[�l������

void monju::DeviceKernel::_create(Device& device, std::string source_path, std::string kernel_name, params_map& params)
{
	_source_path = source_path;
	_kernel_name = kernel_name;
	_p_device = &device;
	_initKernel(device.getClContext(), device.getClDeviceId(), source_path, kernel_name, params);
}

// �f�o�C�X��ɔz�u�ς݂̃J�[�l���v���O���������s
// �����͎��O�ɐݒ肵�Ă����K�v������

void monju::DeviceKernel::_run(cl_int dim, const size_t* global_work_size, const size_t* local_work_size)
{
	cl_int error_code = clEnqueueNDRangeKernel(
		_p_device->getClCommandQueue(),	// OpenCL�L���[
		_kernel,						// �J�[�l��
		dim,							// 2����
		nullptr,						// global work offset
		global_work_size,				// global work size
		local_work_size,				// local work size
		0,								// number of events in wait list
		nullptr,						// event wait list
		nullptr);						// event
	if (error_code != CL_SUCCESS)
		throw OpenClException(error_code);
}

// �v���O�����R���p�C��(CL�t�@�C��)�A�J�[�l������

// �v���O�����R���p�C��(CL�t�@�C��)�A�J�[�l������

// OpenCL�J�[�l������


// OpenCL�J�[�l������

void monju::DeviceKernel::create(Device& device, std::string source_path, std::string kernel_name, params_map& params)
{
	_create(device, source_path, kernel_name, params);
}

void monju::DeviceKernel::release()
{
	if (_kernel != nullptr)
	{
		clReleaseKernel(_kernel);
		_kernel = nullptr;
	}
	if (_program != nullptr)
	{
		clReleaseProgram(_program);
		_program = nullptr;
	}
	_p_device = nullptr;
	_source_path = "";
	_kernel_name = "";
}

void monju::DeviceKernel::compute(std::vector<size_t>& global_work_size)
{
	_run(
		(cl_int)global_work_size.size(),
		global_work_size.data(),
		nullptr
	);
}

void monju::DeviceKernel::compute(std::vector<size_t>& global_work_size, std::vector<size_t>& local_work_size)
{
	if (global_work_size.size() != local_work_size.size())
		throw MonjuException("global_work_size��local_work_size�̎�������v���Ȃ�");
	_run(
		(cl_int)global_work_size.size(),
		global_work_size.data(),
		local_work_size.data()
	);
}
