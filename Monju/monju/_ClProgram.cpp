#include "_ClProgram.h"
#include "OpenClException.h"
#include "util_str.h"
#include "_ClContext.h"

std::string monju::_ClProgram::_get_program_build_info(cl_program program, std::vector<cl_device_id> deviceIds)
{
	std::string message;
	for (auto device_id : deviceIds)
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

std::string monju::_ClProgram::_read_kernel_source_from_file(std::string soucePath)
{
	std::ifstream input(soucePath, std::ifstream::in | std::ifstream::binary);
	std::string cl_source_original((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
	return cl_source_original;
}

std::string monju::_ClProgram::_specialize_source(std::string filePath, param_map params)
{
	std::string plain_source = _read_kernel_source_from_file(filePath);
	std::string edited_source = util_str::parameterizePlaceholders(plain_source, params);
	return edited_source;
}

cl_program monju::_ClProgram::_create_kernel_program(cl_context context, std::string& editedSource)
{
	// OpenCLのプログラムを生成
	const char* p_source = editedSource.c_str();
	const size_t source_size = editedSource.size();
	cl_int error_code;
	cl_program program = clCreateProgramWithSource(context, 1, (const char**)&p_source, (const size_t*)&source_size, &error_code);
	if (error_code != CL_SUCCESS)
		throw OpenClException(error_code);
	return program;
}

cl_program monju::_ClProgram::_compile_kernel_program(cl_context context, std::vector<cl_device_id>& deviceIds, std::string filePath, param_map& params)
{
	// OpenCLカーネルソースを取得し、プレースホルダを値に置換する
	std::string edited_source = _specialize_source(filePath, params);
	cl_program program = _create_kernel_program(context, edited_source);
	cl_int error_code = clBuildProgram(program, static_cast<cl_uint>(deviceIds.size()), deviceIds.data(), nullptr, nullptr, nullptr);
	// コンパイルに失敗した場合はエラー内容を表示
	if (error_code != CL_SUCCESS)
	{
		std::string message = _get_program_build_info(program, deviceIds);
		throw OpenClException(error_code, message);
	}
	return program;
}

void monju::_ClProgram::_create(cl_context context, std::vector<cl_device_id> deviceIds, std::string sourcePath, param_map params)
{
	_program = _compile_kernel_program(context, deviceIds, sourcePath, params);
}

void monju::_ClProgram::_release()
{
	if (_program == nullptr)
		return;
	cl_int error_code = clReleaseProgram(_program);
	if (error_code != CL_SUCCESS)
		throw OpenClException(error_code);
	_program = nullptr;
}

monju::_ClProgram::_ClProgram(cl_context context, std::vector<cl_device_id> deviceIds, std::string sourcePath, param_map params)
{
	_context = context;
	_deviceIds = deviceIds;
	_sourcePath = sourcePath;
	_params = params;
	_create(context, deviceIds, sourcePath, params);
}

monju::_ClProgram::~_ClProgram()
{
	_release();
}

cl_program monju::_ClProgram::clProgram() const
{
	return _program;
}
