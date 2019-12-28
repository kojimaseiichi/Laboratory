// 文字列操作ユーティリティ
#pragma once
#ifndef _MONJU_UTIL_STR_H__
#define _MONJU_UTIL_STR_H__

#include <string>
#include <map>
#include <boost/regex.hpp>

namespace monju {
	namespace util_str {

		// 文字列を置換
		template<class T, class U>
		std::string replace(std::string s, const T& target, const U& replacement, bool replace_first = 0, bool replace_empty = 0) {
			using S = std::string;
			using C = std::string::value_type;
			using N = std::string::size_type;
			struct {
				auto len(const S& s) { return s.size(); }
				auto len(const C* p) { return std::char_traits<C>::length(p); }
				auto len(const C  c) { return 1; }
				auto sub(S* s, const S& t, N pos, N len) { s->replace(pos, len, t); }
				auto sub(S* s, const C* t, N pos, N len) { s->replace(pos, len, t); }
				auto sub(S* s, const C  t, N pos, N len) { s->replace(pos, len, 1, t); }
				auto ins(S* s, const S& t, N pos) { s->insert(pos, t); }
				auto ins(S* s, const C* t, N pos) { s->insert(pos, t); }
				auto ins(S* s, const C  t, N pos) { s->insert(pos, 1, t); }
			} util;

			N target_length = util.len(target);
			N replacement_length = util.len(replacement);
			if (target_length == 0) {
				if (!replace_empty || replacement_length == 0) return s;
				N n = s.size() + replacement_length * (1 + s.size());
				s.reserve(!replace_first ? n : s.size() + replacement_length);
				for (N i = 0; i < n; i += 1 + replacement_length) {
					util.ins(&s, replacement, i);
					if (replace_first) break;
				}
				return s;
			}

			N pos = 0;
			while ((pos = s.find(target, pos)) != std::string::npos) {
				util.sub(&s, replacement, pos, target_length);
				if (replace_first) return s;
				pos += replacement_length;
			}
			return s;
		}

		// ${A}の形式のプレースホルダーを置換
		std::string parameterizePlaceholders(std::string source, std::map<std::string, std::string>& params);

	}
}

#endif // _MONJU_UTIL_STR_H__