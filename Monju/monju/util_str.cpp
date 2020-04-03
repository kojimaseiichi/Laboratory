#include "util_str.h"

// ${A}の形式のプレースホルダーを置換
std::string monju::util_str::parameterizePlaceholders(std::string source, param_map& params)
{
	auto callback = [&params](boost::match_results<std::string::const_iterator> const& m)->std::string {
		std::string key = m[1];
		auto itr = params.find(key);
		if (itr != params.end())
			return itr->second;
		return "";
	};
	boost::basic_regex<char> reg(R"(\$\{([a-zA-Z_]+)\})");
	return boost::regex_replace(source, reg, callback);
}
