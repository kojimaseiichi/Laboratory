#include "DeviceProgram.h"

// �v���O�����r���h���̏����擾

std::string monju::DeviceProgram::_getProgramBuildInfo(cl_program program, std::vector<cl_device_id> device_id_set)
{
	std::string message;
	for (auto device_id : device_id_set)
	{
		size_t logSize;
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize);
		char* buildLog = (char*)malloc((logSize + 1));
		clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, logSize, buildLog, nullptr);
		message.append(buildLog == nullptr ? "" : buildLog);
		message.append("\r\n");
		free(buildLog);
	}
	return message;
}

// �J�[�l���\�[�X�̓��e�𕶎���Ŏ擾

std::string monju::DeviceProgram::_getSource(std::string souce_path)
{
	std::ifstream input(souce_path, std::ifstream::in | std::ifstream::binary);
	std::string cl_source_original((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
	return cl_source_original;
}

// �J�[�l���\�[�X���̃e���v���[�g�ϐ�����̉�

std::string monju::DeviceProgram::_parameterize(std::string source, params_map& params)
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

// �v���[�X�z���_�u���ς݃\�[�X�R�[�h���擾

std::string monju::DeviceProgram::_getEditedSource(std::string file_path, params_map params)
{
	std::string plain_source = _getSource(file_path);
	std::string edited_source = _parameterize(plain_source, params);
	return edited_source;
}

// �v���O�������쐬

cl_program monju::DeviceProgram::_createProgram(cl_context context, std::string& edited_source)
{
	const char* p_source = edited_source.c_str();
	const size_t source_size = edited_source.size();
	cl_int error_code;
	cl_program program = clCreateProgramWithSource(context, 1, (const char**)&p_source, (const size_t*)&source_size, &error_code);
	if (error_code != CL_SUCCESS)
		throw OpenClException(error_code);
	return program;
}

// �J�[�l���R���p�C�����ăJ�[�l���v���O�����𐶐��i���̏�Ԃł͎��s�ł��Ȃ��j
// OpenCL�J�[�l���\�[�X��ǂݍ��݁A�e���v���[�g�ϐ��̋�̉��A�J�[�l���R���p�C��

cl_program monju::DeviceProgram::_compileProgram(cl_context context, std::vector<cl_device_id>& device_id_set, std::string file_path, params_map& params)
{
	// OpenCL�J�[�l���\�[�X���擾���A�v���[�X�z���_��l�ɒu������
	std::string edited_source = _getEditedSource(file_path, params);
	// �v���O�����̃R���p�C��
	cl_program program = _createProgram(context, edited_source);
	cl_int error_code = clBuildProgram(program, static_cast<cl_uint>(device_id_set.size()), device_id_set.data(), nullptr, nullptr, nullptr);
	// �R���p�C���Ɏ��s�����ꍇ�̓G���[���e��\��
	if (error_code != CL_SUCCESS)
	{
		std::string message = _getProgramBuildInfo(program, device_id_set);
		throw OpenClException(error_code, message);
	}
	return program;
}

// �v���O�����R���p�C��(CL�t�@�C��)�A�J�[�l������

void monju::DeviceProgram::_create(DeviceContext& context, std::vector<Device*>& device_set, std::string source_path, params_map& params)
{
	_p_context = &context;
	_device_set = device_set;
	_source_path = source_path;
	std::vector<cl_device_id> device_id_set;
	for (auto const& e : device_set)
		device_id_set.push_back(e->getClDeviceId());
	_program = _compileProgram(context.getContext(), device_id_set, source_path, params);
}

// �v���O������������

void monju::DeviceProgram::create(DeviceContext& context, std::vector<Device*>& device_set, std::string cl_file_path, params_map& params)
{
	_create(context, device_set, cl_file_path, params);
}
