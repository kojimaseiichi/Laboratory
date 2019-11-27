#include "DeviceKernelBase.h"
#include "MonjuException.h" 
#include <fstream>
#include <map>
#include <boost/regex.hpp>

monju::DeviceKernelBase::DeviceKernelBase()
{
	_program = nullptr;
}


monju::DeviceKernelBase::~DeviceKernelBase()
{
	release();
}


// �J�[�l���R���p�C�����ăJ�[�l���v���O�����𐶐��i���̏�Ԃł͎��s�ł��Ȃ��j
// OpenCL�J�[�l���\�[�X��ǂݍ��݁A�e���v���[�g�ϐ��̋�̉��A�J�[�l���R���p�C��

inline cl_program monju::DeviceKernelBase::_compileProgram(cl_context context, cl_device_id device_id, std::string file_path, params_map params)
{
	std::string plain_source = _getSource(file_path);
	std::string edited_source = _editSource(plain_source, params);

	const char* p_source = edited_source.c_str();
	const size_t source_size = edited_source.size();

	// �v���O�����̃R���p�C��
	cl_program program = clCreateProgramWithSource(context, 1, (const char**)&p_source, (const size_t*)&source_size, nullptr);
	cl_int err = clBuildProgram(program, 1, &device_id, nullptr, nullptr, nullptr);
	// �R���p�C���Ɏ��s�����ꍇ�̓G���[���e��\��
	if (err != CL_SUCCESS)
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

// �R���p�C���ς݃J�[�l���v���O�������f�o�C�X�ɔz�u���Ď��s�\�ȏ�ԂɑJ��

inline cl_kernel monju::DeviceKernelBase::_createKernel(cl_program program, std::string kernel_name)
{
	cl_kernel kernel = clCreateKernel(program, kernel_name.c_str(), NULL);
	if (kernel == nullptr)
		throw MonjuException("�J�[�l���̍쐬�Ɏ��s");
	return kernel;
}

// �J�[�l�������s�\�ȏ�ԂɑJ�ځi_compileProgram�A_createKernel�j

inline void monju::DeviceKernelBase::_initKernel(cl_context context, cl_device_id device_id, std::string source_path, std::string kernel_name, params_map params)
{
	_program = _compileProgram(context, device_id, source_path, params);
	_kernel = _createKernel(_program, kernel_name);
}

// �J�[�l���\�[�X�̓��e�𕶎���Ŏ擾

inline std::string monju::DeviceKernelBase::_getSource(std::string souce_path)
{
	std::ifstream input(souce_path, std::ifstream::in | std::ifstream::binary);
	std::string cl_source_original((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
	return cl_source_original;
}

std::string monju::DeviceKernelBase::_editSource(std::string source, std::map<std::string, std::string> params)
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

inline void monju::DeviceKernelBase::_create(GpuDeviceContext& context, Device& device, std::string source_path, std::string kernel_name)
{
	_source_path = source_path;
	_kernel_name = kernel_name;
	_p_dc = &context;
	_p_device = &device;
}

inline cl_int monju::DeviceKernelBase::_run(cl_int dim, const size_t* global_work_size, const size_t* local_work_size)
{
	return clEnqueueNDRangeKernel(
		_p_device->getCommandQueue(),	// OpenCL�L���[
		_kernel,						// �J�[�l��
		dim,							// 2����
		nullptr,						// global work offset
		global_work_size,				// global work size
		local_work_size,				// local work size
		0,								// number of events in wait list
		nullptr,						// event wait list
		nullptr);						// event
}

inline void monju::DeviceKernelBase::_setWorkItem(std::vector<size_t>& global_work_size)
{
	_global_work_size = global_work_size;
	_local_work_size.resize(0);
}

inline void monju::DeviceKernelBase::_setWorkItem(std::vector<size_t>& global_work_size, std::vector<size_t>& local_work_size)
{
	if (global_work_size.size() != local_work_size.size())
		throw MonjuException("");
	_global_work_size = global_work_size;
	_local_work_size = local_work_size;
}

inline void monju::DeviceKernelBase::release()
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
	_p_dc = nullptr;
	_p_device = nullptr;
	_source_path = "";
	_kernel_name = "";
}

