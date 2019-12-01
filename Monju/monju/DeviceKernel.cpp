#include "DeviceKernel.h"
#include "MonjuException.h" 
#include <fstream>
#include <map>
#include <boost/regex.hpp>

monju::DeviceKernel::DeviceKernel()
{
	_program = nullptr;
}


monju::DeviceKernel::~DeviceKernel()
{
	release();
}


// カーネルコンパイルしてカーネルプログラムを生成（この状態では実行できない）
// OpenCLカーネルソースを読み込み、テンプレート変数の具体化、カーネルコンパイル

inline cl_program monju::DeviceKernel::_compileProgram(cl_context context, cl_device_id device_id, std::string file_path, params_map params)
{
	std::string plain_source = _getSource(file_path);
	std::string edited_source = _editSource(plain_source, params);

	const char* p_source = edited_source.c_str();
	const size_t source_size = edited_source.size();

	// プログラムのコンパイル
	cl_program program = clCreateProgramWithSource(context, 1, (const char**)&p_source, (const size_t*)&source_size, nullptr);
	cl_int err = clBuildProgram(program, 1, &device_id, nullptr, nullptr, nullptr);
	// コンパイルに失敗した場合はエラー内容を表示
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

// コンパイル済みカーネルプログラムをデバイスに配置して実行可能な状態に遷移

inline cl_kernel monju::DeviceKernel::_createKernel(cl_program program, std::string kernel_name)
{
	cl_kernel kernel = clCreateKernel(program, kernel_name.c_str(), NULL);
	if (kernel == nullptr)
		throw MonjuException("カーネルの作成に失敗");
	return kernel;
}

// カーネルを実行可能な状態に遷移（_compileProgram、_createKernel）

inline void monju::DeviceKernel::_initKernel(cl_context context, cl_device_id device_id, std::string source_path, std::string kernel_name, params_map params)
{
	_program = _compileProgram(context, device_id, source_path, params);
	_kernel = _createKernel(_program, kernel_name);
}

// カーネルソースの内容を文字列で取得

inline std::string monju::DeviceKernel::_getSource(std::string souce_path)
{
	std::ifstream input(souce_path, std::ifstream::in | std::ifstream::binary);
	std::string cl_source_original((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
	return cl_source_original;
}

std::string monju::DeviceKernel::_editSource(std::string source, std::map<std::string, std::string> params)
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

// プログラムコンパイル(CLファイル)、カーネル生成

inline void monju::DeviceKernel::_create(DeviceContext& context, Device& device, std::string source_path, std::string kernel_name)
{
	_source_path = source_path;
	_kernel_name = kernel_name;
	_p_dc = &context;
	_p_device = &device;
}

inline cl_int monju::DeviceKernel::_run(cl_int dim, const size_t* global_work_size, const size_t* local_work_size)
{
	return clEnqueueNDRangeKernel(
		_p_device->getCommandQueue(),	// OpenCLキュー
		_kernel,						// カーネル
		dim,							// 2次元
		nullptr,						// global work offset
		global_work_size,				// global work size
		local_work_size,				// local work size
		0,								// number of events in wait list
		nullptr,						// event wait list
		nullptr);						// event
}

inline void monju::DeviceKernel::create(DeviceContext& context, Device& device, std::string source_path, std::string kernel_name)
{
	_create(context, device, source_path, kernel_name);
}

inline void monju::DeviceKernel::release()
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

inline void monju::DeviceKernel::compute(std::vector<size_t>& global_work_size)
{
	_run(
		(cl_int)global_work_size.size(),
		global_work_size.data(),
		nullptr
	);
}

inline void monju::DeviceKernel::compute(std::vector<size_t>& global_work_size, std::vector<size_t>& local_work_size)
{
	if (global_work_size.size() != local_work_size.size())
		throw MonjuException("global_work_sizeとlocal_work_sizeの次元が一致しない");
	_run(
		(cl_int)global_work_size.size(),
		global_work_size.data(),
		local_work_size.data()
	);
}
