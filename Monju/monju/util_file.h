// �t�@�C�����[�e�B���e�B
#pragma once
#ifndef _MONJU_UTIL_FILE_H__
#define _MONJU_UTIL_FILE_H__

#include <string>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/serialization.hpp>

namespace monju {
	namespace util_file {

		// �t�@�C���̑��݃`�F�b�N
		bool existsFile(const std::string& path);

		// �t�@�C���̃p�X���擾
		std::string combine(const std::string& dir, const std::string& name, const std::string& extention);

		std::string combine(const std::string& dir, const std::string& name);

		// �t�@�C���̓��e���擾
		std::string readContent(const std::string& path);

		// �V���A����
		template <typename T>
		void serialize(const std::string& path, std::string root, T& obj)
		{
			std::ofstream file(path);
			boost::archive::text_oarchive ar(file);
			ar << boost::serialization::make_nvp(root.c_str(), obj);
		}

		// �t�V���A����
		template <typename T>
		void deserialize(const std::string& path, std::string root, T& obj)
		{
			std::ifstream file(path);
			boost::archive::text_iarchive ar(file);
			ar >> boost::serialization::make_nvp(root.c_str(), obj);
		}
	}
}

#endif // _MONJU_UTIL_FILE_H__
