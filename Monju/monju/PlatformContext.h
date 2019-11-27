#pragma once
#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/filesystem.hpp>

#ifndef _MONJU_PLATFORM_CONTEXT_H__
#define _MONJU_PLATFORM_CONTEXT_H__

namespace monju {

	class PlatformContext
	{
	private:
		const char* kJsonWorkFolder = "work_folder";

	private:
		boost::filesystem::path _work_folder;


	private:
		// �v���p�e�B�t�@�C���ւ̃p�X
		std::string full_path_to_prpperties_json() const
		{
			boost::filesystem::path full_path = _work_folder / "properties.json";
			return full_path.string();
		}
		// �v���b�g�t�H�[������JSON�`���Ńt�@�C���ɏ�������
		void saveJson() const
		{
			boost::property_tree::ptree pt;
			pt.put(kJsonWorkFolder, _work_folder);

			boost::property_tree::write_json(full_path_to_prpperties_json(), pt);
		}
		// �v���b�g�t�H�[�������t�@�C������ǂݍ���
		void loadJson()
		{
			boost::property_tree::ptree pt;
			boost::property_tree::read_json(full_path_to_prpperties_json(), pt);

			//_work_folder = pt.get<std::string>(kJsonWorkFolder);
		}



		// �����E������
	public:
		PlatformContext()
		{
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
			loadJson();
		}
		// �v���b�g�t�H�[�������t�@�C���ɕۑ�����
		void close()
		{
			saveJson();
		}
	};

}

#endif // !_MONJU_PLATFORM_CONTEXT_H__
