#pragma once
#ifndef _MONJU__CL_PROGRAM_H__
#define _MONJU__CL_PROGRAM_H__

#include <vector>
#include <map>
#include <string>
#include <CL/cl.h>

namespace monju
{
	using param_map = std::map<std::string, std::string>;

	class _ClProgram
	{
	private:
		cl_context _context;
		std::vector<cl_device_id> _deviceIds;
		std::string _sourcePath;
		param_map _params;
		cl_program _program;	// 解放予定

	private:
		std::string _get_program_build_info(cl_program program, std::vector<cl_device_id> deviceIds);
		std::string _read_kernel_source_from_file(std::string soucePath);
		std::string _specialize_source(std::string filePath, param_map params);
		cl_program _create_kernel_program(cl_context context, std::string& editedSource);
		cl_program _compile_kernel_program(cl_context context, std::vector<cl_device_id>& deviceIds, std::string filePath, param_map& params);
		void _create(cl_context context, std::vector<cl_device_id> deviceIds, std::string sourcePath, param_map params);
		void _release();

	public:
		_ClProgram(cl_context context, std::vector<cl_device_id> deviceIds, std::string sourcePath, param_map params);
		~_ClProgram();

	public:
		cl_program clProgram() const;

		// コピー禁止・ムーブ禁止
	public:
		_ClProgram(const _ClProgram&) = delete;
		_ClProgram(_ClProgram&&) = delete;
		_ClProgram& operator=(const _ClProgram&) = delete;
		_ClProgram& operator=(_ClProgram&&) = delete;
	};
}

#endif // !_MONJU__CL_PROGRAM_H__
