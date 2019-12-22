#pragma once
#ifndef _MONJU_DEVICE_PROGRAM_H__
#define _MONJU_DEVICE_PROGRAM_H__

#include <vector>
#include <map>
#include <fstream>
#include <CL/cl.h>
#include <boost/regex.hpp>
#include "DeviceContext.h"
#include "OpenClException.h"
#include "util_str.h"

namespace monju {

	// OpenCL�v���O����
	class DeviceProgram
	{
	public:
		using params_map = std::map<std::string, std::string>;

	private:

		DeviceContext*		_p_context;
		std::vector<Device*> _device_set;
		cl_program			_program;
		std::string			_source_path;	// CL�t�@�C����

	private:
		// �v���O�����r���h���̏����擾
		std::string			_getProgramBuildInfo(
			cl_program program,
			std::vector<cl_device_id> device_id_set);

		// �J�[�l���\�[�X�̓��e�𕶎���Ŏ擾
		std::string			_getSource(
			std::string souce_path);

		// �v���[�X�z���_�u���ς݃\�[�X�R�[�h���擾
		std::string			_getEditedSource(
			std::string file_path,
			params_map params);

		// �v���O�������쐬
		cl_program			_createProgram(
			cl_context context,
			std::string& edited_source);

		// �J�[�l���R���p�C�����ăJ�[�l���v���O�����𐶐��i���̏�Ԃł͎��s�ł��Ȃ��j
		// OpenCL�J�[�l���\�[�X��ǂݍ��݁A�e���v���[�g�ϐ��̋�̉��A�J�[�l���R���p�C��
		cl_program			_compileProgram(
			cl_context context,
			std::vector<cl_device_id>& device_id_set,
			std::string file_path,
			params_map& params);

		// �v���O�����R���p�C��(CL�t�@�C��)�A�J�[�l������
		void				_create(
			DeviceContext& context,
			std::vector<Device*>& device_set,
			std::string source_path,
			params_map& params);

	public:

		DeviceProgram();

		~DeviceProgram();

		DeviceProgram(const DeviceProgram&) = delete;

		DeviceProgram& operator=(const DeviceProgram&) = delete;

		// �v���O������������
		void	create(
			DeviceContext& context,
			std::vector<Device*>& device_set,
			std::string cl_file_path,
			params_map& params);

		void	release();

	public: // �v���p�e�B
		DeviceContext&	deviceContext() const { return *_p_context; }
		cl_program		program() const { return _program; }
		std::string		clFileName() const { return _source_path; }
		std::vector<Device*>	deviceSet() const { return _device_set; }
	};

} // namespace monju

#endif // !_MONJU_DEVICE_PROGRAM_H__
