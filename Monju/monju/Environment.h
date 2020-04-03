#pragma once
#ifndef _MONJU_ENVIRONMENT_H__
#define _MONJU_ENVIRONMENT_H__

#include "MonjuTypes.h"

namespace monju
{
	class Environment
	{
	public:
		struct Info
		{
			std::string workFolder;
			std::string kernelFolder;
			int platformId;
		};
	private:
		Info _info;
		std::string _workFolder;

		// �v���p�e�B�t�@�C���ւ̃p�X
		std::string _propertyFilePath(std::string workFolder) const;
		// �v���b�g�t�H�[������JSON�`���Ńt�@�C���ɏ�������
		void _saveJson(const std::string workFolder) const;
		// �v���b�g�t�H�[�������t�@�C������ǂݍ���
		void _loadJson(const std::string workFolder);

	public:
		Environment(std::string workFolder);
		~Environment();
		void open(std::string workFolder);
		Info info() const;

		// �R�s�[�֎~�E���[�u�֎~
	public:
		Environment(const Environment&) = delete;
		Environment(Environment&&) = delete;
		Environment& operator=(const Environment&) = delete;
		Environment& operator=(Environment&&) = delete;
	};
}

#endif // !_MONJU_ENVIRONMENT_H__
