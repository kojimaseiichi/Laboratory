#pragma once
#ifndef _MONJU__CL_PROGRAM_H__
#define _MONJU__CL_PROGRAM_H__

#include <CL/cl.h>
#include <string>
#include <vector>
#include <fstream>
#include "OpenClException.h"
#include "util_str.h"
#include "_ClContext.h"

namespace monju
{
	class _ClProgram
	{
	public:
		using params_map = std::map<std::string, std::string>;

	private:
		cl_context _context;
		std::vector<cl_device_id> _deviceIds;
		std::string _sourcePath;
		params_map _params;
		cl_program _program;	// 解放予定

		std::string _getProgramBuildInfo(cl_program program, std::vector<cl_device_id> deviceIds);
		std::string _getSource(std::string soucePath);
		std::string _getEditedSource(std::string filePath, params_map params);
		cl_program _createProgram(cl_context context, std::string& editedSource);
		cl_program _compileProgram(cl_context context, std::vector<cl_device_id>& deviceIds, std::string filePath, params_map& params);
		void _create(cl_context context, std::vector<cl_device_id> deviceIds, std::string sourcePath, params_map params);
		void _release();


	public:
		_ClProgram(cl_context context, std::vector<cl_device_id> deviceIds, std::string sourcePath, params_map params);
		~_ClProgram();
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
