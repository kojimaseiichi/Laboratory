#include "Environment.h"

// �v���p�e�B�t�@�C���ւ̃p�X

std::string monju::Environment::_propertyFilePath(std::string workFolder) const
{
	boost::filesystem::path bwf = boost::filesystem::path(workFolder);
	boost::filesystem::path full_path = bwf / "Environment.json";
	return full_path.string();
}

// �v���b�g�t�H�[������JSON�`���Ńt�@�C���ɏ�������

void monju::Environment::_saveJson(const std::string workFolder) const
{
	std::string fileName = _propertyFilePath(workFolder);
	std::ofstream o;
	o.open(fileName, std::ofstream::out | std::ofstream::binary);
	nlohmann::json j;
	j["workFolder"] = _info.workFolder;
	j["kernelFolder"] = _info.kernelFolder;
	j["platformId"] = _info.platformId;
	o << j;
}

// �v���b�g�t�H�[�������t�@�C������ǂݍ���

void monju::Environment::_loadJson(const std::string workFolder)
{
	std::string fileName = _propertyFilePath(workFolder);
	std::ifstream i;
	i.open(fileName, std::ofstream::in | std::ofstream::binary);
	nlohmann::json j;
	i >> j;
	_info = {
		j["workFolder"].get<std::string>(),
		j["kernelFolder"].get<std::string>(),
		j["platformId"].get<int>()
	};
}

monju::Environment::Environment(std::string workFolder)
{
	open(workFolder);
}

monju::Environment::~Environment()
{
}

void monju::Environment::open(std::string workFolder)
{
	_workFolder = workFolder;
	_loadJson(workFolder);
}

monju::Environment::Info monju::Environment::info() const
{
	return _info;
}
