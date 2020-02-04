#pragma once
#ifndef _MONJU_CL_PROGRAM_H__
#define _MONJU_CL_PROGRAM_H__

#include <CL/cl.h>
#include <string>
#include <vector>
#include <fstream>
#include "OpenClException.h"
#include "util_str.h"
#include "ClContext.h"

namespace monju
{
	class ClProgram
	{
	public:
		using params_map = std::map<std::string, std::string>;

	private:
		ClContext* _pContext;
		std::string _sourcePath;
		params_map _params;
		cl_program _program;	// 解放予定

		std::string _getProgramBuildInfo(cl_program program, std::vector<cl_device_id> deviceIds)
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
		std::string _getSource(std::string soucePath)
		{
			std::ifstream input(soucePath, std::ifstream::in | std::ifstream::binary);
			std::string cl_source_original((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
			return cl_source_original;
		}
		std::string _getEditedSource(std::string filePath, params_map params)
		{
			std::string plain_source = _getSource(filePath);
			std::string edited_source = util_str::parameterizePlaceholders(plain_source, params);
			return edited_source;
		}
		cl_program _createProgram(cl_context context, std::string& editedSource)
		{
			const char* p_source = editedSource.c_str();
			const size_t source_size = editedSource.size();
			cl_int error_code;
			cl_program program = clCreateProgramWithSource(context, 1, (const char**)&p_source, (const size_t*)&source_size, &error_code);
			if (error_code != CL_SUCCESS)
				throw OpenClException(error_code);
			return program;
		}
		cl_program _compileProgram(cl_context context, std::vector<cl_device_id>& deviceIds, std::string filePath, params_map& params)
		{
			// OpenCLカーネルソースを取得し、プレースホルダを値に置換する
			std::string edited_source = _getEditedSource(filePath, params);
			// プログラムのコンパイル
			cl_program program = _createProgram(context, edited_source);
			cl_int error_code = clBuildProgram(program, static_cast<cl_uint>(deviceIds.size()), deviceIds.data(), nullptr, nullptr, nullptr);
			// コンパイルに失敗した場合はエラー内容を表示
			if (error_code != CL_SUCCESS)
			{
				std::string message = _getProgramBuildInfo(program, deviceIds);
				throw OpenClException(error_code, message);
			}
			return program;
		}
		void _create(cl_context context, std::vector<cl_device_id> deviceIds, std::string sourcePath, params_map params)
		{
			_program = _compileProgram(context, deviceIds, sourcePath, params);
		}
		void _release()
		{
			if (_program == nullptr)
				return;
			cl_int error_code = clReleaseProgram(_program);
			if (error_code != CL_SUCCESS)
				throw OpenClException(error_code);
			_program = nullptr;
		}


	public:
		ClProgram(ClContext& context, std::string sourcePath, params_map params)
		{
			_pContext = &context;
			_sourcePath = sourcePath;
			_params = params;
			_create(context.context, context.deviceIds, sourcePath, params);
		}
		~ClProgram()
		{
			_release();
		}
		cl_program program() const
		{
			return _program;
		}


		// コピー禁止・ムーブ禁止
	public:
		ClProgram(const ClProgram&) = delete;
		ClProgram(ClProgram&&) = delete;
		ClProgram& operator=(const ClProgram&) = delete;
		ClProgram& operator=(ClProgram&&) = delete;
	};
}

#endif // !_MONJU_CL_PROGRAM_H__
