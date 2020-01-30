#include "util_file.h"
#include "util_str.h"

#include <fstream>

bool monju::util_file::existsFile(const std::string & path)
{
	std::ifstream s(path.c_str());
	return s.is_open();
}

std::string monju::util_file::combine(const std::string & dir, const std::string & name, const std::string & extention)
{
	auto back = dir.back();
	if (back == '\\' || back == '/')
		return dir + name + "." + extention;
	return dir + "\\" + name + "." + extention;
}

std::string monju::util_file::combine(const std::string& dir, const std::string& name)
{
	auto back = dir.back();
	if (back == '\\' || back == '/')
		return dir + name;
	return dir + "\\" + name;
}

std::string monju::util_file::readContent(const std::string & path)
{
	std::ifstream ifs(path);
	std::string content;
	content.assign(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
	return content;
}

