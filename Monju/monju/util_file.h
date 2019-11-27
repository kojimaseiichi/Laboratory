// �t�@�C�����[�e�B���e�B
#pragma once
#ifndef _MONJU_UTIL_FILE_H__
#define _MONJU_UTIL_FILE_H__

#include <string>

namespace monju {
	namespace util_file {

		// �t�@�C���̑��݃`�F�b�N
		bool existsFile(const std::string& path);

		// �t�@�C���̃p�X���擾
		std::string combine(const std::string& dir, const std::string& name, const std::string& extention);

		// �t�@�C���̓��e���擾
		std::string readContent(const std::string& path);
	}
}

#endif // _MONJU_UTIL_FILE_H__
