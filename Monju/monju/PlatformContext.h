#pragma once
#ifndef _MONJU_PLATFORM_CONTEXT_H__
#define _MONJU_PLATFORM_CONTEXT_H__

#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/filesystem.hpp>

#include "monju/DeviceContext.h"

namespace monju {

	class PlatformContext
	{
	private:
		boost::filesystem::path _work_folder;
		boost::filesystem::path _kernel_folder;
		int _platform_id;

	private:
		std::unique_ptr<DeviceContext> _device_context;

	private:
		// �v���p�e�B�t�@�C���ւ̃p�X
		std::string full_path_to_prpperties_json(std::string work_folder) const;
		// �v���b�g�t�H�[������JSON�`���Ńt�@�C���ɏ�������
		void saveJson(const std::string work_folder) const;
		// �v���b�g�t�H�[�������t�@�C������ǂݍ���
		void loadJson(const std::string work_folder);
		// 
		void createDeviceContext(int platform_id);



		// �����E������
	public:
		PlatformContext();
		~PlatformContext();

		// ���J�֐�
	public:
		// �v���b�g�t�H�[�������t�@�C������ǂݍ��ݗ��p�\�Ƃ���
		void open(std::string work_folder);
		void create(std::string work_folder, std::string kernel_folder, int platform_id);
		// �v���b�g�t�H�[�������t�@�C���ɕۑ�����
		void close();

		// �v���p�e�B
	public:
		// ���[�N�X�y�[�X�f�B���N�g��
		std::string workspaceDir();
		std::string kernelDir();
		// �f�o�C�X�R���e�L�X�g�i�v�Z�����j
		DeviceContext& deviceContext() const;

	};

}

#endif // !_MONJU_PLATFORM_CONTEXT_H__
