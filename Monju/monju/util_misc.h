// その他のユーティリティ
#pragma once
#ifndef _MONJU_UTIL_MISC_H__
#define _MONJU_UTIL_MISC_H__

#include <vector>

namespace monju {
	namespace util_misc {

		/// <summary>列挙型のビット表現をベクトルに変更</summary>
		template <typename Enum>
		std::vector<Enum> split_enum(Enum e)
		{
			std::vector<Enum> v;
			for (int n = 1; n <= 32; n++)
			{
				Enum flag = static_cast<Enum>(1 << n);
				if (e & flag)
					v.push_back(flag);
			}
			return v;
		}
	}
}

#endif // !_MONJU_UTIL_MISC_H__
