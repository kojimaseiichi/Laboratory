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
		int _platform_id;

	private:
		std::unique_ptr<DeviceContext> _device_context;

	private:
		// �v���p�e�B�t�@�C���ւ̃p�X
		std::string full_path_to_prpperties_json(std::string work_folder) const
		{
			boost::filesystem::path bwf = boost::filesystem::path(work_folder);
			boost::filesystem::path full_path = bwf / "properties.json";
			return full_path.string();
		}
		// �v���b�g�t�H�[������JSON�`���Ńt�@�C���ɏ�������
		void saveJson(const std::string work_folder) const
		{
			boost::property_tree::ptree root;
			// root
			root.put("work_folder", work_folder);
			{
				// deivce
				boost::property_tree::ptree device;
				device.put("platform_id", _platform_id);
				root.add_child("device", device);
			}
			boost::property_tree::write_json(full_path_to_prpperties_json(work_folder), root);
		}
		// �v���b�g�t�H�[�������t�@�C������ǂݍ���
		void loadJson(const std::string work_folder)
		{
			boost::property_tree::ptree root;
			boost::property_tree::read_json(full_path_to_prpperties_json(work_folder), root);
			// root
			_work_folder = work_folder;
			{
				// deivce
				auto device = root.get_child("device");
				_platform_id = device.get<int>("platform_id");
			}
		}

		void createDeviceContext(int platform_id)
		{
			_device_context = std::make_unique<DeviceContext>();
			_device_context->create(platform_id);
		}



		// �����E������
	public:
		PlatformContext()
		{
			_work_folder = "";
			_platform_id = 0;
		}
		~PlatformContext()
		{
		}

		// ���J�֐�
	public:
		// �v���b�g�t�H�[�������t�@�C������ǂݍ��ݗ��p�\�Ƃ���
		void open(std::string work_folder)
		{
			_work_folder = work_folder;
			loadJson(work_folder);
			createDeviceContext(_platform_id);
		}
		void create(std::string work_folder, int platform_id)
		{
			_work_folder = work_folder;
			_platform_id = platform_id;
			saveJson(work_folder);
			createDeviceContext(_platform_id);
		}
		// �v���b�g�t�H�[�������t�@�C���ɕۑ�����
		void close()
		{
			saveJson(_work_folder.string());
		}

		// �v���p�e�B
	public:
		// ���[�N�X�y�[�X�f�B���N�g��
		std::string workspaceDir()
		{
			return _work_folder.string();
		}

		// �f�o�C�X�R���e�L�X�g�i�v�Z�����j
		DeviceContext& deviceContext() const
		{
			return *_device_context;
		}

	};

}

#endif // !_MONJU_PLATFORM_CONTEXT_H__
